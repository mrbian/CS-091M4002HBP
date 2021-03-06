//
// Created by BMAN on 2019/5/23.
//

#include "base.h"
#include "arpcache.h"
#include "arp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpcache.h>

// 生成一个路由端口
iface_info_t * iface_init() {
    iface_info_t *iface = (iface_info_t *)malloc(sizeof(iface_info_t));
    bzero(iface, sizeof(iface_info_t));
    init_list_head(&iface->list);

    u32 ip4 = 0xA001;  // 10.0.0.1
    u32 mask = 0xF000;  // 255.0.0.0
    u8 mac[ETH_ALEN] = {1, 2, 3, 4, 5, 6};  // 01-01-01-01-01-01
    char *name = "节点1（网关）";
    char *ip_str = "10.0.0.1";

    iface->fd = 1;
    iface->index = 1;
    memcpy(iface->mac, mac, sizeof(u8) * ETH_ALEN);
    memcpy(iface->name, name, sizeof(char) * strlen(name));
    memcpy(iface->ip_str, ip_str, sizeof(char) * strlen(ip_str));
    return iface;
}

// arpcache_test
void arpcache_test(){
    // init system
    iface_info_t *iface = iface_init();
    arpcache_init();

    // init packet1
    u32 ip4 = 0xA0000001;  // 10.0.0.1
    char *ip_str = "10.0.0.1";
    u8 mac[ETH_ALEN] = {1, 2, 3, 4, 5, 6};  // 01-01-01-01-01-01
    char *packet = "test1";         // todo: 要用正确的struct来生成packet

    int result = arpcache_lookup(ip4, mac);  // should be not found
    if(result == 0) {
        printf("未找到IP为%s的MAC地址映射，应将此包加入待决包列表\n", ip_str);
        arpcache_append_packet(iface, ip4, packet, (int)strlen(packet));
    }

    // 再加两个包
    ip4 = 0xA002;  // 10.0.0.2
    packet = "test2";
    arpcache_append_packet(iface, ip4, packet, (int)strlen(packet));
    ip4 = 0xA001;  // 10.0.0.1
    packet = "test3";
    arpcache_append_packet(iface, ip4, packet, (int)strlen(packet));
    print_arp_cache_list();

    // 新建一个cache，测试arpcache_insert函数
    arpcache_insert(ip4, mac);
    print_arp_cache_list();
}

void arp_test() {
    iface_info_t *iface = iface_init();
    u32 dst_ip = 0xA0000001;            // todo: dst_ip is -160313131 ?
    arp_send_request(iface, dst_ip);
}