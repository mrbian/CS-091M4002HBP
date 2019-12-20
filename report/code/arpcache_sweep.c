void *arpcache_sweep(void *arg)
{
    int i = 0;
    time_t now;
    while (1) {
        sleep(1);
        // traverse arp cache, delete item which has been staying over 15s
        for(i = 0; i < MAX_ARP_SIZE; i += 1) {
            time(&now);
            if((long)now - (long)arpcache.entries[i].added > 15) {
                arpcache.entries[i].valid = 0;
            }
        }

        // traverse packet list, if waiting time is more than 1s, re-send it
        // if retries over 5 times, send icmp packet
        struct arp_req *req = NULL, *req_q;
        struct cached_pkt *pkt = NULL, *pkt_q;
        list_for_each_entry_safe(req, req_q, &(arpcache.req_list), list) {
            if(req->retries >= 5) {
                pkt = NULL;
                list_for_each_entry_safe(pkt, pkt_q, &(req->cached_packets), list) {
                    printf("arp request failed, send icmp packet\n");
                    icmp_send_packet(pkt->packet, pkt->len, 3, 1);
                    list_delete_entry(&(pkt->list));
                }
                list_delete_entry(&(req->list));
            } else {
                time(&now);
                if((long)now - (long)req->sent >= 1) {
                    printf("arp request retry one more time\n");
                    req->retries += 1;
                    arp_send_request(req->iface, req->ip4);
                } else {
                    printf("now is %ld, req->sent is %ld \n", (long)now, (long)req->sent);
                }
            }
        }
    }

    return NULL;
}
