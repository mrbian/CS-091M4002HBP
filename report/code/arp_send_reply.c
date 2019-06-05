void arp_send_reply(iface_info_t *iface, struct ether_arp *req_hdr)
{
    // 根据arp请求内容包装好一个arp回复包，并把它发送出去
    // 应答时，新包的源ip和源mac里就是查询结果
    char *packet = (char *)malloc(sizeof(struct ether_header) + sizeof(struct ether_arp));
    bzero(packet, sizeof(struct ether_header) + sizeof(struct ether_arp));

    struct ether_header *eh = (struct ether_header *)packet;
    struct ether_arp *ea = packet_to_arp_hdr(packet);

    // 设置各项的值
    memcpy(eh->ether_shost, iface->mac, ETH_ALEN);
    memcpy(eh->ether_dhost, req_hdr->arp_sha, ETH_ALEN);
    eh->ether_type = htons(ETH_P_ARP);

    ea->arp_hrd = htons(1);
    ea->arp_pro = htons(0x0800);
    ea->arp_hln = ETH_ALEN;
    ea->arp_pln = 4;
    ea->arp_op = htons(ARPOP_REPLY);

    ea->arp_tpa = htonl(req_hdr->arp_spa);
    memcpy(ea->arp_tha, req_hdr->arp_sha, ETH_ALEN);
    ea->arp_spa = htonl(req_hdr->arp_tpa);

    // 先看是否是查询本机的ip地址
    // 若是，则直接填充好发出
    // 若不是，则查询
    if(req_hdr->arp_tpa == iface->ip) {
        memcpy(ea->arp_sha, iface->mac, ETH_ALEN);
        iface_send_packet(iface, packet, (int)(sizeof(struct ether_header) + sizeof(struct ether_arp)));
    } else {
        int found = arpcache_lookup(req_hdr->arp_tpa, req_hdr->arp_tha);
        if(found) {
            iface_send_packet(iface, packet, (int)(sizeof(struct ether_header) + sizeof(struct ether_arp)));
        } else {
            free(packet);
        }
    }
}
