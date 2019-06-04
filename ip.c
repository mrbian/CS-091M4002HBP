#include "ip.h"
#include "icmp.h"
#include "packet.h"
#include "arpcache.h"
#include "rtable.h"
#include "arp.h"

// #include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <rtable.h>

// initialize ip header 
void ip_init_hdr(struct iphdr *ip, u32 saddr, u32 daddr, u16 len, u8 proto)
{
	ip->version = 4;
	ip->ihl = 5;				// 总长度20字节
	ip->tos = 0;
	ip->tot_len = htons(len);
	ip->id = rand();
	ip->frag_off = htons(IP_DF);
	ip->ttl = DEFAULT_TTL;
	ip->protocol = proto;
	ip->saddr = htonl(saddr);
	ip->daddr = htonl(daddr);
	ip->checksum = ip_checksum(ip);			// 这里校验和没有进行大小字节转换
}

// lookup in the routing table, to find the entry with the same and longest prefix.
// the input address is in host byte order
rt_entry_t *longest_prefix_match(u32 dst)
{
//	fprintf(stderr, "TODO: longest prefix match for the packet.\n");
	rt_entry_t * result = NULL;
	int max_mask = 0;    // 最大子网掩码

	rt_entry_t * ele = NULL;
	list_for_each_entry(ele, &rtable, list) {
		if((ele->dest & ele->mask) == (dst & ele->mask)) {				// 如果按位与有相同的
			if(ele->mask >= max_mask) {							// 如果本子网掩码是最大的
				max_mask = ele->mask;
				result = ele;
			}
		}
	}

	return result;
}

// send IP packet
//
// Different from ip_forward_packet, ip_send_packet sends packet generated by
// router itself. This function is used to send ICMP packets.
void ip_send_packet(char *packet, int len)
{
	fprintf(stderr, "TODO: send ip packet.\n");

}
