#ifndef __BASE_H__
#define __BASE_H__

#include "types.h"
#include "ether.h"
#include "list.h"

#include <arpa/inet.h>

typedef struct {
	struct list_head iface_list;  // 所有的网络端口，以链表为组织
	int nifs;
	struct pollfd *fds;
} ustack_t;

extern ustack_t *instance;  // 全局实例

// todo: 这里将typedef struct -> typedef struct iface_info_t
typedef struct iface_info_t{   // 网络端口信息
	struct list_head list;

	int fd;
	int index;
	u8	mac[ETH_ALEN];	// MAC地址
	u32 ip;    			// IPv4地址
	u32 mask;  			// 子网掩码
	char name[16];		// 端口名称
	char ip_str[16];	// 打印debug用
} iface_info_t;

#endif
