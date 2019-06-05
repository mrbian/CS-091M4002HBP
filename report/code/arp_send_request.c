void arp_send_request(iface_info_t *iface, u32 dst_ip)
{
    // 包装好一个arp请求包，并且把它发出去（注意要把以太网首部也带上）
    char *packet = (char *)malloc(sizeof(struct ether_header) + sizeof(struct ether_arp));
    bzero(packet, sizeof(struct ether_header) + sizeof(struct ether_arp));

    struct ether_header *eh = (struct ether_header *)packet;
    struct ether_arp *ea = packet_to_arp_hdr(packet);

    // 设置各项的值
    // ether
    memcpy(eh->ether_shost, iface->mac, ETH_ALEN);
    int i;
    for(i = 0; i < ETH_ALEN; i += 1) {
        eh->ether_dhost[i] = 0xFF;
    }
    eh->ether_type = htons(ETH_P_ARP);

    // arp
    ea->arp_hrd = htons(1);
    ea->arp_pro = htons(0x0800);
    ea->arp_hln = ETH_ALEN;
    ea->arp_pln = 4;
    ea->arp_op = htons(ARPOP_REQUEST);

    ea->arp_spa = htonl(iface->ip);
    memcpy(ea->arp_sha, iface->mac, ETH_ALEN);
    ea->arp_tpa = htonl(dst_ip);
    // tha置空（00 00 00 00 00 00）
    printf("arp request for target ip address: %x is sending \n", dst_ip);

    // 发送出去
    iface_send_packet(iface, packet, (int)(sizeof(struct ether_header) + sizeof(struct ether_arp)));
}
