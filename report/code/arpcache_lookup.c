int arpcache_lookup(u32 ip4, u8 mac[ETH_ALEN]) {
    // 遍历ARP表，尝试找到ip4对应的mac地址，若找到且结果有效，返回1；若未找到，返回0；
    int i = 0;
    int flag = 0;
    for (i = 0; i < MAX_ARP_SIZE; i += 1) {
        if(arpcache.entries[i].ip4 == ip4 && arpcache.entries[i].valid == 1) {
            flag = 1;
            memcpy(mac, arpcache.entries[i].mac, sizeof(u8) * ETH_ALEN);
        }
    }
    return flag;
}
