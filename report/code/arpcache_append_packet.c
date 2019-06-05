void arpcache_append_packet(iface_info_t *iface, u32 ip4, char *packet, int len)
{
    // （若IPv4地址对应MAC地址在本主机未找到）将包加入ARP缓存
    // 若缓存中已有对应IPv4地址的包，则将包插入对应链表；
    // 若缓存中没有，则创建链表；

    int flag = 0;
    struct arp_req *ele = NULL;
    list_for_each_entry(ele, &arpcache.req_list, list) {
        if (ele->ip4 == ip4) {
            flag = 1;
            struct cached_pkt *new_pkt = (struct cached_pkt *)malloc(sizeof(struct cached_pkt));
            new_pkt->packet = (char *)malloc((size_t)len);
            memcpy(new_pkt->packet, packet, len);
            new_pkt->len = len;
            init_list_head(&new_pkt->list);
            list_add_tail(&new_pkt->list, &ele->cached_packets);
        }
    }

    // 若未找到
    if(flag == 0) {
        // 先新建缓存对象
        struct arp_req *new_req = (struct arp_req *)malloc(sizeof(struct arp_req));
        // 给缓存对象初始化值
        new_req->ip4 = ip4;
        new_req->retries = 0;
        new_req->iface = iface;
        init_list_head(&new_req->cached_packets);
        init_list_head(&new_req->list);
        list_add_tail(&new_req->list, &arpcache.req_list);
        // 再新建包对象并加入缓存对象的链表
        struct cached_pkt *new_pkt = (struct cached_pkt *)malloc(sizeof(struct cached_pkt));
        new_pkt->packet = (char *)malloc((size_t)len);
        memcpy(new_pkt->packet, packet, len);
        new_pkt->len = len;
        init_list_head(&new_pkt->list);
        list_add_tail(&new_pkt->list, &new_req->cached_packets);

        arp_send_request(iface, ip4);
    }
}