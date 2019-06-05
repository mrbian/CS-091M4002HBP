void handle_ip_packet(iface_info_t *iface, char *packet, int len)
{
    struct iphdr *ip = packet_to_ip_hdr(packet);
    u32 daddr = ntohl(ip->daddr);
    if (daddr == iface->ip) {
        icmp_send_packet(packet, len, 0, 0);
        free(packet);
    }
    else {
        ip_forward_packet(daddr, packet, len);
    }
}