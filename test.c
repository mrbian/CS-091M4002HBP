//
// Created by BMAN on 2019/5/23.
//

#include "base.h"
#include "arpcache.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpcache.h>

// 生成一个路由端口
iface_info_t * iface_init() {
    struct iface_info_t *iface = (struct iface_info_t *)malloc(sizeof(struct iface_info_t));
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

// 遍历并打印所有的缓存包
void print_arp_cache_list() {
    printf("print all cached packet \n");
    struct arp_req *req = NULL;
    struct cached_pkt *pkt = NULL;
    list_for_each_entry(req, &arpcache.req_list, list) {
        pkt = NULL;
//        list_for_each_entry(pkt, &req->cached_packets, list) {
//            printf("Packet is %s \n", pkt->packet);
//            printf("what the fuck");
//        }
    }
}

// arpcache_test
void arpcache_test(){
    // init system
    struct iface_info_t *iface = iface_init();
    arpcache_init();

    // init packet
    u32 ip4 = 0xA001;  // 10.0.0.1
    char *ip_str = "10.0.0.1";
    u8 mac[ETH_ALEN] = {1, 2, 3, 4, 5, 6};  // 01-01-01-01-01-01
    char *packet = "test packet";

    int result = arpcache_lookup(ip4, mac);  // should be not found
    if(result == 0) {
        printf("未找到IP为%s的MAC地址映射，应将此包加入待决包列表\n", ip_str);
        arpcache_append_packet(iface, ip4, packet, (int)strlen(packet));
    }
    print_arp_cache_list();
}