void arpcache_append_packet(iface_info_t *iface, u32 ip4, char *packet, int len)
{
    // append a packet into arp cache packet list
    // if there has been one list with given ipv4 addr, insert packet into that list
    // if not, create a list

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

    // if not found
    if(flag == 0) {
        // firstly, create cache object
        struct arp_req *new_req = (struct arp_req *)malloc(sizeof(struct arp_req));
        // init value
        new_req->ip4 = ip4;
        new_req->retries = 0;
        new_req->iface = iface;
        init_list_head(&new_req->cached_packets);
        init_list_head(&new_req->list);
        list_add_tail(&new_req->list, &arpcache.req_list);
        // create packet object and insert it into list
        struct cached_pkt *new_pkt = (struct cached_pkt *)malloc(sizeof(struct cached_pkt));
        new_pkt->packet = (char *)malloc((size_t)len);
        memcpy(new_pkt->packet, packet, len);
        new_pkt->len = len;
        init_list_head(&new_pkt->list);
        list_add_tail(&new_pkt->list, &new_req->cached_packets);

        arp_send_request(iface, ip4);
    }
}
