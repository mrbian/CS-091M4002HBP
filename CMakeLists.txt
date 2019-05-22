cmake_minimum_required(VERSION 3.9)
project(2_router)

set(CMAKE_CXX_STANDARD 11)

add_executable(2_router
        include/arp.h
        include/arpcache.h
        include/base.h
        include/checksum.h
        include/ether.h
        include/hash.h
        include/icmp.h
        include/ip.h
        include/list.h
        include/log.h
        include/packet.h
        include/rtable.h
        include/types.h
        arp.c
        arpcache.c
        icmp.c
        ip.c
        ip_forwarding.c
        main.c
        packet.c
        rtable.c
        rtable_internal.c)
