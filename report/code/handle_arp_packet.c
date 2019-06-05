void handle_arp_packet(iface_info_t *iface, char *packet, int len)
{
    struct ether_arp * ea = packet_to_arp_hdr(packet);
    // 进行字节序转换
    ea->arp_op = ntohs(ea->arp_op);
    ea->arp_tpa = ntohl(ea->arp_tpa);
    ea->arp_spa = ntohl(ea->arp_spa);
    // 无论是arp请求还是arp回复，都要将源放入ARP表
    arpcache_insert(ea->arp_spa, ea->arp_sha);
    if(ea->arp_op == ARPOP_REQUEST) {
        // 给予arp回复
        arp_send_reply(iface, ea);
    }
    free(packet);
}