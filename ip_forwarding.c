#include "ip.h"
#include "icmp.h"
#include "rtable.h"
#include "arp.h"
#include "arpcache.h"

#include <stdio.h>
#include <stdlib.h>
#include <rtable.h>

// forward the IP packet from the interface specified by longest_prefix_match, 
// when forwarding the packet, you should check the TTL, update the checksum,
// determine the next hop to forward the packet, then send the packet by 
// iface_send_packet_by_arp
void ip_forward_packet(u32 ip_dst, char *packet, int len)
{
	fprintf(stderr, "TODO: forward ip packet.\n");
	// check TTL
	struct ether_header * eh = (struct ether_header *)packet;
	struct iphdr * pkt_ip_hdr = packet_to_ip_hdr(packet);
	if(pkt_ip_hdr->ttl <= 0) {
		free(packet);
		return;
	}

	// forward packet
	rt_entry_t * rt = longest_prefix_match(ip_dst);
	if(rt) {
		// update checksum and ttl
		pkt_ip_hdr->ttl -= 1;
		pkt_ip_hdr->checksum = ip_checksum(pkt_ip_hdr);  // 最后设置checksum
		printf("iface->ip %x \n", rt->iface->ip);
		iface_send_packet_by_arp(rt->iface, rt->gw == 0 ? pkt_ip_hdr->daddr : rt->gw, packet, len);
	} else {
		free(packet);
		return;
	}
}

// handle ip packet
//
// If the packet is ICMP echo request and the destination IP address is equal to
// the IP address of the iface, send ICMP echo reply; otherwise, forward the
// packet.
void handle_ip_packet(iface_info_t *iface, char *packet, int len)
{
	struct iphdr *ip = packet_to_ip_hdr(packet);
	u32 daddr = ntohl(ip->daddr);
	if (daddr == iface->ip) {
		fprintf(stderr, "TODO: reply to the sender if it is ping packet.\n");

		icmp_send_packet(packet, len, 0, 0);
		free(packet);
	}
	else {
		ip_forward_packet(daddr, packet, len);
	}
}
