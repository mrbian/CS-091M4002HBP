#include "icmp.h"
#include "ip.h"
#include "rtable.h"
#include "arp.h"
#include "base.h"

#include <stdio.h>
#include <stdlib.h>
#include <rtable.h>
#include <base.h>

// send icmp packet
// 新建一个icmp包利用ip_send_packet发出去
// const是告诉coder，这里不能free和修改
// 为什么会有第一个参数？不是应该根据type、code、dst_ip直接发吗？
// 如果第一个参数是纯数据的话，应该有dst_ip参数，但是没有，所以第一个参数是别的包(⊙﹏⊙)b
// todo: 这里应该只设置icmp的包头，将ip包头的部分的工作交给ip_send_packet来完成？
void icmp_send_packet(const char *in_pkt, int len, u8 type, u8 code)
{
	fprintf(stderr, "TODO: malloc and send icmp packet.\n");
	// 获取到ip目的地址
	struct iphdr * pkt_ip_hdr = packet_to_ip_hdr(in_pkt);
	u32 dst_ip = pkt_ip_hdr->daddr;

	rt_entry_t * rt = longest_prefix_match(dst_ip);
	if(rt) {
		// malloc an icmp packet
		// ip header
		size_t packet_length = ETHER_HDR_SIZE + sizeof(struct iphdr) + sizeof(struct icmphdr);
		char * packet = (char *)malloc(packet_length);
		struct iphdr * packet_iphdr = packet_to_ip_hdr(packet);
		ip_init_hdr(packet_iphdr, rt->iface->ip, dst_ip, sizeof(struct iphdr) + sizeof(struct icmphdr), 1);   // protocal IPPROTO_ICMP : 1

		// icmp header
		struct icmphdr * packet_icmphdr = (struct icmphdr *)(packet + ETHER_HDR_SIZE + packet_iphdr->ihl);
		packet_icmphdr->code = code;
		packet_icmphdr->type = type;
		packet_icmphdr->checksum = icmp_checksum(packet_icmphdr, sizeof(struct icmphdr));		// 要在最后面设置checksum

		// send
		iface_send_packet_by_arp(rt->iface, dst_ip, packet, (int)packet_length);
	}
}
