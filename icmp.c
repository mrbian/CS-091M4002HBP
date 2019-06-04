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
// 根据in_pkt（发送方的包）新建一个icmp包发出去
// const是告诉coder，这里不能free和修改
// 为什么会有第一个参数？不是应该根据type、code、dst_ip直接发吗？
// 如果第一个参数是纯数据的话，应该有dst_ip参数，但是没有，所以第一个参数是别的包(⊙﹏⊙)b
void icmp_send_packet(const char *in_pkt, int len, u8 type, u8 code)
{
//	fprintf(stderr, "TODO: malloc and send icmp packet.\n");
	// 获取到ip目的地址
	struct iphdr * pkt_ip_hdr = packet_to_ip_hdr(in_pkt);
	struct icmphdr * pkt_icmp_hdr;
	pkt_ip_hdr->saddr = ntohl(pkt_ip_hdr->saddr);			// 不只是这里会用到，其他函数也会用到，因此要先进行字节序转换
	pkt_ip_hdr->daddr = ntohl(pkt_ip_hdr->daddr);

	rt_entry_t * rt = longest_prefix_match(pkt_ip_hdr->saddr);
	if(rt) {
		// malloc an icmp packet
		// ip header
		size_t packet_length = ETHER_HDR_SIZE + sizeof(struct iphdr) + sizeof(struct icmphdr);
		char * packet = (char *)malloc(packet_length);
		struct iphdr * packet_iphdr = packet_to_ip_hdr(packet);
		ip_init_hdr(packet_iphdr, rt->iface->ip, pkt_ip_hdr->saddr, sizeof(struct iphdr) + sizeof(struct icmphdr), 1);   // protocal IPPROTO_ICMP : 1

		// icmp header
		struct icmphdr * packet_icmphdr = (struct icmphdr *)(packet + ETHER_HDR_SIZE + packet_iphdr->ihl * 4);
		packet_icmphdr->code = code;
		packet_icmphdr->type = type;

		// 判断类型对数据域进行分类处理
		switch(type) {
			case 0:
				pkt_icmp_hdr = (struct icmphdr *)(in_pkt + ETHER_HDR_SIZE + pkt_ip_hdr->ihl * 4);
				packet_icmphdr->icmp_identifier = pkt_icmp_hdr->icmp_identifier;
				packet_icmphdr->icmp_sequence = pkt_icmp_hdr->icmp_sequence;
				break;
			case 3:
				break;
			default:
				printf("unknown icmp type! \n");
				break;
		}

		packet_icmphdr->checksum = icmp_checksum(packet_icmphdr, sizeof(struct icmphdr));		// 要在最后面设置checksum
        iface_send_packet_by_arp(rt->iface, pkt_ip_hdr->saddr, packet, (int)packet_length);		// 发送
	}
}
