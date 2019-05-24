#include "arpcache.h"
#include "arp.h"
#include "ether.h"
#include "packet.h"
#include "icmp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpcache.h>

static arpcache_t arpcache;  // todo: 使用static修饰以后，就只能在本源文件中可见，并不能在整个工程中可见。
							 //		  如果不加static，所有函数，变量默认都是extern修饰的。可以在其他文件中被引入。

// initialize IP->mac mapping, request list, lock and sweeping thread
void arpcache_init()
{
	bzero(&arpcache, sizeof(arpcache_t));

	init_list_head(&(arpcache.req_list));   // todo: 难道说malloc了一个struct，内部的各种结构体都自动malloc了？

	pthread_mutex_init(&arpcache.lock, NULL);

	pthread_create(&arpcache.thread, NULL, arpcache_sweep, NULL);
}

// release all the resources when exiting
void arpcache_destroy()
{
	pthread_mutex_lock(&arpcache.lock);

	struct arp_req *req_entry = NULL, *req_q;
	list_for_each_entry_safe(req_entry, req_q, &(arpcache.req_list), list) {
		struct cached_pkt *pkt_entry = NULL, *pkt_q;
		list_for_each_entry_safe(pkt_entry, pkt_q, &(req_entry->cached_packets), list) {
			list_delete_entry(&(pkt_entry->list));
			free(pkt_entry->packet);
			free(pkt_entry);
		}

		list_delete_entry(&(req_entry->list));
		free(req_entry);
	}

	pthread_kill(arpcache.thread, SIGTERM);

	pthread_mutex_unlock(&arpcache.lock);
}

// lookup the IP->mac mapping
//
// traverse the table to find whether there is an entry with the same IP
// and mac address with the given arguments
int arpcache_lookup(u32 ip4, u8 mac[ETH_ALEN])
{
	fprintf(stderr, "TODO: lookup ip address in arp cache.\n");
    // 遍历ARP表，尝试找到ip4对应的mac地址
    // 若找到，将mac数组各变量替换后返回1；
    // 若未找到，返回0；
    int i = 0;
    int j = 0;
    int flag = 0;  // 默认未找到: 0
    for (i = 0; i < MAX_ARP_SIZE; i += 1) {
//        printf("%d \n", arpcache.entries[i].ip4);
        if(arpcache.entries[i].ip4 == ip4) {
            flag = 1;                           // flag置为找到: 1
            memcpy(arpcache.entries[i].mac, mac, sizeof(u8) * ETH_ALEN);  // MAC地址赋值
        }
    }

	return flag;
}

// append the packet to arpcache
//
// Lookup in the list which stores pending packets, if there is already an
// entry with the same IP address and iface (which means the corresponding arp
// request has been sent out), just append this packet at the tail of that entry
// (the entry may contain more than one packet); otherwise, malloc a new entry
// with the given IP address and iface, append the packet, and send arp request.
// len是packet这个内容的长度
void arpcache_append_packet(iface_info_t *iface, u32 ip4, char *packet, int len)
{
	fprintf(stderr, "TODO: append the ip address if lookup failed, and send arp request if necessary.\n");
    // （若IPv4地址对应MAC地址在本主机未找到）将包加入ARP缓存
    // 若缓存中已有对应IPv4地址的包，则将包插入对应链表；
    // 若缓存中没有，则创建链表；

    int flag = 0; // 默认未找到
    // 遍历ARP缓存链表中的第一层链表
    struct arp_req *ele = NULL;
    list_for_each_entry(ele, &arpcache.req_list, list) {            // 注意这里是&arpcache.req_list而不是arpcache.req_list，因为宏定义里面用的是->操作符，所以必须传入结构体指针，而不是结构体变量
        if (ele->ip4 == ip4) {  // 若找到
            flag = 1;
            struct cached_pkt *new_pkt = (struct cached_pkt *)malloc(sizeof(struct cached_pkt));
            new_pkt->packet = packet;
            new_pkt->len = len;
            struct list_head * new = (struct list_head *)malloc(sizeof(struct list_head));                     // 新建指针
            new_pkt->list = *new;
            list_add_tail(new, &ele->cached_packets);                                                           // 将包对象串上去
        }
    }

    // 若未找到
    if(flag == 0) {
        // 先新建缓存对象
        struct arp_req *new_req = (struct arp_req *)malloc(sizeof(struct arp_req));     // 新建缓存对象
        init_list_head(&new_req->list);
        list_add_tail(&new_req->list, &arpcache.req_list);                                         // 将缓存对象串上去

        // 再新建包对象并加入缓存对象的链表
        init_list_head(&new_req->cached_packets);                                       // 初始化包节点
        struct cached_pkt *new_pkt = (struct cached_pkt *)malloc(sizeof(struct cached_pkt));
        new_pkt->packet = packet;
        new_pkt->len = len;
        init_list_head(&new_pkt->list);
        list_add_tail(&new_pkt->list, &new_req->cached_packets);                                   // 将包对象串上去

        // todo: send arp request

    }
}

// insert the IP->mac mapping into arpcache, if there are pending packets
// waiting for this mapping, fill the ethernet header for each of them, and send
// them out
void arpcache_insert(u32 ip4, u8 mac[ETH_ALEN])
{
	fprintf(stderr, "TODO: insert ip->mac entry, and send all the pending packets.\n");
    // 向ARP表中插入一个IP到MAC地址映射条目
    // 遍历缓存列表，如果有对应IP地址的待决包，则发送出去

}

// sweep arpcache periodically
//
// For the IP->mac entry, if the entry has been in the table for more than 15
// seconds, remove it from the table.
// For the pending packets, if the arp request is sent out 1 second ago, while 
// the reply has not been received, retransmit the arp request. If the arp
// request has been sent 5 times without receiving arp reply, for each
// pending packet, send icmp packet (DEST_HOST_UNREACHABLE), and drop these
// packets.
void *arpcache_sweep(void *arg) 
{
	while (1) {
		sleep(1);
		fprintf(stderr, "TODO: sweep arpcache periodically: remove old entries, resend arp requests .\n");
	}

	return NULL;
}


// 遍历并打印所有的缓存包
void print_arp_cache_list() {
	printf("print all cached packet \n");
	struct arp_req *req = NULL;
	struct cached_pkt *pkt = NULL;
	list_for_each_entry(req, &arpcache.req_list, list) {
        pkt = NULL;
        list_for_each_entry(pkt, &req->cached_packets, list) {
            printf("Packet is %s \n", pkt->packet);
            printf("what the fuck");
        }
	}
}
