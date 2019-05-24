#include "arp.h"
#include "base.h"
#include "types.h"
#include "packet.h"
#include "ether.h"
#include "arpcache.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "log.h"

// send an arp request: encapsulate an arp request packet, send it out through
// iface_send_packet
void arp_send_request(iface_info_t *iface, u32 dst_ip)
{
	fprintf(stderr, "TODO: send arp request when lookup failed in arpcache.\n");
    // 包装好一个arp请求包，并且把它发出去
	struct ether_arp *arp_req_pkt = (struct ether_arp *)malloc(sizeof(struct ether_arp));
	char *packet = (char *)malloc(sizeof(struct ether_arp));
//	arp_pkt->arp_hrd = iface->mac;
	arp_req_pkt->arp_op = 0x01;
	int i;
	for(i = 0; i < ETH_ALEN; i += 1) {
		arp_req_pkt->arp_tha[i] = 0xFF;   // 各项全为FF
	}
	memcpy(arp_req_pkt->arp_sha, iface->mac, ETH_ALEN);

	arp_req_pkt->arp_spa = iface->ip;
	arp_req_pkt->arp_tpa = dst_ip;
	arp_req_pkt->arp_hln = ETH_ALEN;
	arp_req_pkt->arp_pln = sizeof(dst_ip);   // todo: 这里是protocol address length？
	memcpy(packet, arp_req_pkt, sizeof(struct ether_arp));
	printf("%s", packet);
}

// send an arp reply packet: encapsulate an arp reply packet, send it out
// through iface_send_packet
void arp_send_reply(iface_info_t *iface, struct ether_arp *req_hdr)
{
	fprintf(stderr, "TODO: send arp reply when receiving arp request.\n");
}

void handle_arp_packet(iface_info_t *iface, char *packet, int len)
{
	fprintf(stderr, "TODO: process arp packet: arp request & arp reply.\n");
}

// send (IP) packet through arpcache lookup 
//
// Lookup the mac address of dst_ip in arpcache. If it is found, fill the
// ethernet header and emit the packet by iface_send_packet, otherwise, pending 
// this packet into arpcache, and send arp request.
void iface_send_packet_by_arp(iface_info_t *iface, u32 dst_ip, char *packet, int len)
{
	struct ether_header *eh = (struct ether_header *)packet;
	memcpy(eh->ether_shost, iface->mac, ETH_ALEN);
	eh->ether_type = htons(ETH_P_IP);

	u8 dst_mac[ETH_ALEN];
	int found = arpcache_lookup(dst_ip, dst_mac);
	if (found) {
		// log(DEBUG, "found the mac of %x, send this packet", dst_ip);
		memcpy(eh->ether_dhost, dst_mac, ETH_ALEN);
		iface_send_packet(iface, packet, len);
	}
	else {
		// log(DEBUG, "lookup %x failed, pend this packet", dst_ip);
		arpcache_append_packet(iface, dst_ip, packet, len);
	}
}
