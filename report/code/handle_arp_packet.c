void handle_arp_packet(iface_info_t *iface, char *packet, int len)
{
    struct ether_arp * ea = packet_to_arp_hdr(packet);
    // change byte order
    ea->arp_op = ntohs(ea->arp_op);
    ea->arp_tpa = ntohl(ea->arp_tpa);
    ea->arp_spa = ntohl(ea->arp_spa);
    // put the source information into arp table
    arpcache_insert(ea->arp_spa, ea->arp_sha);
    if(ea->arp_op == ARPOP_REQUEST) {
        // give arp reply
        arp_send_reply(iface, ea);
    }
    free(packet);
}
