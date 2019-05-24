#ifndef __ARPCACHE_H__
#define __ARPCACHE_H__

#include "base.h"
#include "types.h"
#include "list.h"

#include <pthread.h>

#define MAX_ARP_SIZE 32
#define ARP_ENTRY_TIMEOUT 15
#define ARP_REQUEST_MAX_RETRIES	5

struct cached_pkt {  // 缓存的包结构
	struct list_head list;
	char *packet;
	int len;
};

struct arp_req {  // 等待ARP回复的包缓存，二维链表结构
	struct list_head list;  // 指向前一个和后一个arp_req的指针结构体
	iface_info_t *iface;
	u32 ip4;
	time_t sent;
	int retries;
	struct list_head cached_packets;  // 自身维护的链表
};

struct arp_cache_entry {  // ARP表中的一个条目
	u32 ip4; 	// stored in host byte order IP地址，本地字节序
	u8 mac[ETH_ALEN];
	time_t added;
	int valid;
};

typedef struct {  // ARP表
	struct arp_cache_entry entries[MAX_ARP_SIZE]; // table行数组
	struct list_head req_list;  // 等待ARP回复的包结构
	pthread_mutex_t lock;  // ARP表维护线程
	pthread_t thread; // ARP表维护线程
} arpcache_t;

static arpcache_t arpcache; // todo: here has been added
void arpcache_init();
void arpcache_destroy();
void *arpcache_sweep(void *);

int arpcache_lookup(u32 ip4, u8 mac[]);
void arpcache_insert(u32 ip4, u8 mac[]);
void arpcache_append_packet(iface_info_t *iface, u32 ip4, char *packet, int len);

#endif
