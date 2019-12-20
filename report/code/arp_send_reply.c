void arp_send_reply(iface_info_t *iface, struct ether_arp *req_hdr)
{
    // encapsulate an arp request packet and send it out
    // when response one packet, it's spa and sha is arp request result
    char *packet = (char *)malloc(sizeof(struct ether_header) + sizeof(struct ether_arp));
    bzero(packet, sizeof(struct ether_header) + sizeof(struct ether_arp));

    struct ether_header *eh = (struct ether_header *)packet;
    struct ether_arp *ea = packet_to_arp_hdr(packet);

    // set value
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

    // firstly, check is this host's ip
    // if it is, fill the ether and send it out
    // if not, find it in arpcache
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
