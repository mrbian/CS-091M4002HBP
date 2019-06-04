#include "arp.h"
#include "base.h"
#include "types.h"
#include "packet.h"
#include "ether.h"
#include "arpcache.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <base.h>

// #include "log.h"

// send an arp request: encapsulate an arp request packet, send it out through
// iface_send_packet
// 一个变量只要超过了一字节，发送前要转换大小端字节序
void arp_send_request(iface_info_t *iface, u32 dst_ip)
{
	fprintf(stderr, "TODO: send arp request when lookup failed in arpcache.\n");
    // 包装好一个arp请求包，并且把它发出去（注意要把以太网首部也带上）
    char *packet = (char *)malloc(sizeof(struct ether_header) + sizeof(struct ether_arp));
    bzero(packet, sizeof(struct ether_header) + sizeof(struct ether_arp));

    struct ether_header *eh = (struct ether_header *)packet;
    struct ether_arp *ea = packet_to_arp_hdr(packet);

    // 设置各项的值
    // ether
    memcpy(eh->ether_shost, iface->mac, ETH_ALEN);  // ether
    int i;
    for(i = 0; i < ETH_ALEN; i += 1) {
		eh->ether_dhost[i] = 0xFF;   // 各项全为FF
	}
    eh->ether_type = htons(ETH_P_ARP);

    // arp
    ea->arp_hrd = htons(1);
    ea->arp_pro = htons(0x0800);
    ea->arp_hln = ETH_ALEN; // 6字节                 // arp
    ea->arp_pln = 4;        // 4字节
    ea->arp_op = htons(ARPOP_REQUEST);

    ea->arp_spa = htonl(iface->ip);
    memcpy(ea->arp_sha, iface->mac, ETH_ALEN);
    ea->arp_tpa = htonl(dst_ip);
    // tha置空（00 00 00 00 00 00）
    printf("arp request for target ip address: %x is sending \n", dst_ip);

    // 发送出去
    iface_send_packet(iface, packet, (int)(sizeof(struct ether_header) + sizeof(struct ether_arp)));
}

// send an arp reply packet: encapsulate an arp reply packet, send it out
// through iface_send_packet
void arp_send_reply(iface_info_t *iface, struct ether_arp *req_hdr)
{
	fprintf(stderr, "TODO: send arp reply when receiving arp request.\n");
    // 根据arp请求内容包装好一个arp回复包，并把它发送出去
    // 应答时，新包的源ip和源mac里就是查询结果
    char *packet = (char *)malloc(sizeof(struct ether_header) + sizeof(struct ether_arp));
    bzero(packet, sizeof(struct ether_header) + sizeof(struct ether_arp));

    struct ether_header *eh = (struct ether_header *)packet;
    struct ether_arp *ea = packet_to_arp_hdr(packet);

    // 设置各项的值
    memcpy(eh->ether_shost, iface->mac, ETH_ALEN);  // ether
    memcpy(eh->ether_dhost, req_hdr->arp_sha, ETH_ALEN);
    eh->ether_type = htons(ETH_P_ARP);

    ea->arp_hrd = htons(1);                         // arp
    ea->arp_pro = htons(0x0800);
    ea->arp_hln = ETH_ALEN; // 6字节
    ea->arp_pln = 4;        // 4字节
    ea->arp_op = htons(ARPOP_REPLY);

    ea->arp_tpa = htonl(req_hdr->arp_spa);
    memcpy(ea->arp_tha, req_hdr->arp_sha, ETH_ALEN);
    ea->arp_spa = htonl(req_hdr->arp_tpa);

    // 先看是否是查询本机的ip地址，若是，则直接填充好发出
    if(req_hdr->arp_tpa == iface->ip) {
        memcpy(ea->arp_sha, iface->mac, ETH_ALEN);
        iface_send_packet(iface, packet, (int)(sizeof(struct ether_header) + sizeof(struct ether_arp)));
    } else {                                            // 若不是，则查询
        int found = arpcache_lookup(req_hdr->arp_tpa, req_hdr->arp_tha);
        if(found) {
            iface_send_packet(iface, packet, (int)(sizeof(struct ether_header) + sizeof(struct ether_arp)));
        } else {
            free(packet);
        }
    }
}

void handle_arp_packet(iface_info_t *iface, char *packet, int len)
{
	fprintf(stderr, "\n\nTODO: process arp packet: arp request & arp reply.\n");
    struct ether_arp * ea = packet_to_arp_hdr(packet);

    // 进行字节序转换
    ea->arp_op = ntohs(ea->arp_op);
    ea->arp_tpa = ntohl(ea->arp_tpa);
    ea->arp_spa = ntohl(ea->arp_spa);

    arpcache_insert(ea->arp_spa, ea->arp_sha);              // 无论是arp请求还是arp回复，都要将源放入ARP表

    if(ea->arp_op == ARPOP_REQUEST) {
        // 给予arp回复
        arp_send_reply(iface, ea);
    }

    free(packet);               // 这里不断收包，要清理内存
}

// send (IP) packet through arpcache lookup 
//
// Lookup the mac address of dst_ip in arpcache. If it is found, fill the
// ethernet header and emit the packet by iface_send_packet, otherwise, pending
// this packet into arpcache, and send arp request.
void iface_send_packet_by_arp(iface_info_t *iface, u32 dst_ip, char *packet, int len)
{
	struct ether_header *eh = (struct ether_header *)packet;        // 可以直接强制转换
	memcpy(eh->ether_shost, iface->mac, ETH_ALEN);
	eh->ether_type = htons(ETH_P_IP);

	u8 dst_mac[ETH_ALEN];
	int found = arpcache_lookup(dst_ip, dst_mac);
	if (found) {
        memcpy(eh->ether_dhost, dst_mac, ETH_ALEN);
        iface_send_packet(iface, packet, len);
	} else {
		arpcache_append_packet(iface, dst_ip, packet, len);             // append_packet里面已send了一次request了
	}
}

