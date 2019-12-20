void icmp_send_packet(const char *in_pkt, int len, u8 type, u8 code)
{
    // get ip destination addr
    struct iphdr * pkt_ip_hdr = packet_to_ip_hdr(in_pkt);
    struct icmphdr * pkt_icmp_hdr = (struct icmphdr *)(in_pkt + ETHER_HDR_SIZE + pkt_ip_hdr->ihl * 4);

    rt_entry_t * rt = longest_prefix_match(ntohl(pkt_ip_hdr->saddr));
    if(rt) {
        size_t packet_length;
        char * packet;
        struct iphdr * packet_iphdr;
        struct icmphdr * packet_icmphdr;
        // judge type and process
        switch(type) {
            case 0:
                // malloc an icmp packet
                // ip header
                packet_length = (size_t)len;
                packet = (char *)malloc(packet_length);
                packet_iphdr = packet_to_ip_hdr(packet);
                ip_init_hdr(packet_iphdr, rt->iface->ip, ntohl(pkt_ip_hdr->saddr), (u16)(packet_length - sizeof(struct ether_header)), 1);
                // icmp header
                packet_icmphdr = (struct icmphdr *)(packet + ETHER_HDR_SIZE + packet_iphdr->ihl * 4);
                packet_icmphdr->code = code;
                packet_icmphdr->type = type;
                packet_icmphdr->icmp_identifier = pkt_icmp_hdr->icmp_identifier;
                packet_icmphdr->icmp_sequence = pkt_icmp_hdr->icmp_sequence;

                memcpy(
                        packet + ETHER_HDR_SIZE + packet_iphdr->ihl * 4 + sizeof(struct icmphdr),
                        in_pkt + ETHER_HDR_SIZE + pkt_ip_hdr->ihl * 4 + sizeof(struct icmphdr),
                        len - ETHER_HDR_SIZE - pkt_ip_hdr->ihl * 4 - sizeof(struct icmphdr)
                );

                packet_icmphdr->checksum = icmp_checksum(packet_icmphdr, (int)packet_length - ETHER_HDR_SIZE - sizeof(struct iphdr));
                break;
            case 3:						// dst unreachable
                // malloc an icmp packet
                // ip header
                packet_length = (size_t)(ETHER_HDR_SIZE + sizeof(struct iphdr) + sizeof(struct icmphdr) + sizeof(struct iphdr) + 8);
                packet = (char *)malloc(packet_length);
                packet_iphdr = packet_to_ip_hdr(packet);
                ip_init_hdr(packet_iphdr, rt->iface->ip, ntohl(pkt_ip_hdr->saddr), (u16)(packet_length - sizeof(struct ether_header)), 1);
                // icmp header
                packet_icmphdr = (struct icmphdr *)(packet + ETHER_HDR_SIZE + packet_iphdr->ihl * 4);
                packet_icmphdr->code = code;
                packet_icmphdr->type = type;

                memcpy(
                        packet + ETHER_HDR_SIZE + packet_iphdr->ihl * 4 + sizeof(struct icmphdr),
                        in_pkt + ETHER_HDR_SIZE,
                        sizeof(struct iphdr) + 8
                );
                packet_icmphdr->checksum = icmp_checksum(packet_icmphdr, (int)packet_length - ETHER_HDR_SIZE - sizeof(struct iphdr));
                break;
            case 8:						// icmp request
                // todo
                printf("this packet icmp type is 8, todo \n");
                break;
            case 11:				// ttl exceed
                // malloc an icmp packet
                // ip header
                packet_length = (size_t)(ETHER_HDR_SIZE + sizeof(struct iphdr) + sizeof(struct icmphdr) + sizeof(struct iphdr) + 8);
                packet = (char *)malloc(packet_length);
                packet_iphdr = packet_to_ip_hdr(packet);
                ip_init_hdr(packet_iphdr, rt->iface->ip, ntohl(pkt_ip_hdr->saddr), (u16)(packet_length - sizeof(struct ether_header)), 1);
                // icmp header
                packet_icmphdr = (struct icmphdr *)(packet + ETHER_HDR_SIZE + packet_iphdr->ihl * 4);
                packet_icmphdr->code = code;
                packet_icmphdr->type = type;

                memcpy(
                        packet + ETHER_HDR_SIZE + packet_iphdr->ihl * 4 + sizeof(struct icmphdr),
                        in_pkt + ETHER_HDR_SIZE,
                        sizeof(struct iphdr) + 8
                );
                packet_icmphdr->checksum = icmp_checksum(packet_icmphdr, (int)packet_length - ETHER_HDR_SIZE - sizeof(struct iphdr));
                break;
            default:
                printf("unknown icmp type! \n");
                return;
        }

        iface_send_packet_by_arp(rt->iface, ntohl(pkt_ip_hdr->saddr), packet, (int)packet_length);
    }
}
