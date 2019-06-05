#include "icmp.h"
#include "ip.h"
#include "rtable.h"
#include "arp.h"
#include "base.h"

#include <stdio.h>
#include <stdlib.h>
#include <rtable.h>
#include <base.h>
#include <string.h>

// send icmp packet
// 根据in_pkt（发送方的包）新建一个icmp包发出去
// const是告诉coder，这里不能free和修改
void icmp_send_packet(const char *in_pkt, int len, u8 type, u8 code)
{
//	fprintf(stderr, "TODO: malloc and send icmp packet.\n");
	// 获取到ip目的地址
	struct iphdr * pkt_ip_hdr = packet_to_ip_hdr(in_pkt);
	struct icmphdr * pkt_icmp_hdr = (struct icmphdr *)(in_pkt + ETHER_HDR_SIZE + pkt_ip_hdr->ihl * 4);

	rt_entry_t * rt = longest_prefix_match(ntohl(pkt_ip_hdr->saddr));
	if(rt) {
		size_t packet_length;
		char * packet;
		struct iphdr * packet_iphdr;
		struct icmphdr * packet_icmphdr;

		// 判断类型对数据域进行分类处理
		switch(type) {
			case 0:						// icmp回复
				// malloc an icmp packet
				// ip header
				packet_length = (size_t)len;  // ICMP协议要将原包的IP的header添加到icmp header的后面，所以有两份ip header的空间
				packet = (char *)malloc(packet_length);
				packet_iphdr = packet_to_ip_hdr(packet);
				ip_init_hdr(packet_iphdr, rt->iface->ip, ntohl(pkt_ip_hdr->saddr), (u16)(packet_length - sizeof(struct ether_header)), 1);   // protocal IPPROTO_ICMP : 1

				// icmp header
				packet_icmphdr = (struct icmphdr *)(packet + ETHER_HDR_SIZE + packet_iphdr->ihl * 4);
				packet_icmphdr->code = code;
				packet_icmphdr->type = type;
				packet_icmphdr->icmp_identifier = ntohs(pkt_icmp_hdr->icmp_identifier);
				packet_icmphdr->icmp_sequence = ntohs(pkt_icmp_hdr->icmp_sequence);

                // 需要request包的所有数据，这里必须是以packet和in_pkt为起始地址，而不能以packet_icmphdr（结构体类型）为起始地址，因为packet和in_pkt是char型指针，移动一个单位是一字节
				memcpy(
                        packet + ETHER_HDR_SIZE + packet_iphdr->ihl * 4 + sizeof(struct icmphdr),
                        in_pkt + ETHER_HDR_SIZE + pkt_ip_hdr->ihl * 4 + sizeof(struct icmphdr),
                        len - ETHER_HDR_SIZE - pkt_ip_hdr->ihl * 4 - sizeof(struct icmphdr)
                );

                // 设置checksum
                packet_icmphdr->checksum = icmp_checksum(packet_icmphdr, sizeof(struct icmphdr));           // checksum自身不需要进行字节序转换，注意是根据本地字节序计算的checksum
                packet_icmphdr->icmp_identifier = htons(pkt_icmp_hdr->icmp_identifier);
                packet_icmphdr->icmp_sequence = htons(pkt_icmp_hdr->icmp_sequence);
				break;
			case 3:						// 目的不可达
				// malloc an icmp packet
				// ip header
				packet_length = (size_t)(ETHER_HDR_SIZE + sizeof(struct iphdr) + sizeof(struct icmphdr) + sizeof(struct iphdr));
				packet = (char *)malloc(packet_length);
				packet_iphdr = packet_to_ip_hdr(packet);
				ip_init_hdr(packet_iphdr, rt->iface->ip, ntohl(pkt_ip_hdr->saddr), (u16)(packet_length - sizeof(struct ether_header)), 1);   // protocal IPPROTO_ICMP : 1

				// icmp header
				packet_icmphdr = (struct icmphdr *)(packet + ETHER_HDR_SIZE + packet_iphdr->ihl * 4);
				packet_icmphdr->code = code;
				packet_icmphdr->type = type;

				memcpy(
                        packet + ETHER_HDR_SIZE + packet_iphdr->ihl * 4 + sizeof(struct icmphdr),
                        in_pkt + ETHER_HDR_SIZE,
                        sizeof(struct iphdr)
                );								// 需要原包的ip header和64bit的数据
				packet_icmphdr->checksum = icmp_checksum(packet_icmphdr, sizeof(struct icmphdr));		// 要在最后面设置checksum
				break;
			case 8:						// icmp请求
				// todo
				break;
			case 11:				// 超时
				// malloc an icmp packet
				// ip header
				packet_length = (size_t)(ETHER_HDR_SIZE + sizeof(struct iphdr) + sizeof(struct icmphdr) + sizeof(struct iphdr));
				packet = (char *)malloc(packet_length);
				packet_iphdr = packet_to_ip_hdr(packet);
				ip_init_hdr(packet_iphdr, rt->iface->ip, ntohl(pkt_ip_hdr->saddr), (u16)(packet_length - sizeof(struct ether_header)), 1);   // protocal IPPROTO_ICMP : 1

				// icmp header
				packet_icmphdr = (struct icmphdr *)(packet + ETHER_HDR_SIZE + packet_iphdr->ihl * 4);
				packet_icmphdr->code = code;
				packet_icmphdr->type = type;

                // 需要原包的ip header和64bit的数据
				memcpy(
                        packet + ETHER_HDR_SIZE + packet_iphdr->ihl * 4 + sizeof(struct icmphdr),
                        in_pkt + ETHER_HDR_SIZE,
                        sizeof(struct iphdr)
                );
				packet_icmphdr->checksum = icmp_checksum(packet_icmphdr, sizeof(struct icmphdr));		// 要在最后面设置checksum
				break;
			default:
				printf("unknown icmp type! \n");
				return;
		}

        iface_send_packet_by_arp(rt->iface, ntohl(pkt_ip_hdr->saddr), packet, (int)packet_length);		// 发送
	}
}
