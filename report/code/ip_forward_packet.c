void ip_forward_packet(u32 ip_dst, char *packet, int len)
{
    // check TTL
    struct iphdr * pkt_ip_hdr = packet_to_ip_hdr(packet);
    // (当一台路由器由到一个TTL值为1的数据包，会直接丢弃)
    if(pkt_ip_hdr->ttl - 1 <= 0) {
        icmp_send_packet(packet, len, 11, 0);
        free(packet);
        return;
    }

    // forward packet
    rt_entry_t * rt = longest_prefix_match(ip_dst);
    if(rt) {
        pkt_ip_hdr->ttl -= 1;
        pkt_ip_hdr->checksum = ip_checksum(pkt_ip_hdr);
        iface_send_packet_by_arp(rt->iface, rt->gw == 0 ? ntohl(pkt_ip_hdr->daddr) : rt->gw, packet, len);
    } else {
        // 返回ICMP Destination Net Unreachable
        icmp_send_packet(packet, len, 3, 0);
        free(packet);
        return;
    }
}