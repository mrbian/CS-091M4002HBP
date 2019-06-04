# 091M4002HBP

## 完成顺序
* arpcache.c
  * arpcache_lookup()
  * arpcache_append_packet()
  * print_arp_cache_list()
  * arpcache_insert()
  * arpcache_sweep()
* test.c
  * arpcache_test () 
* arp.c
  * arp_send_request()
  * arp_send_reply()
  * handle_arp_packet()
  
  
## TODO
* arp表的替换应该是随机替换而不是先入先出。

## 收获

* 一种数据结构，利用宏复用给多种类型（list_head）；
* 如何设置一个网络packet（利用char*与结构体变量的相互转换）；
* 大小字节序真的需要注意（htonl, htons, ntohl, ntohs, n:network, h:host）；
* 应该从上层往底层写，而不是从底层往上层写，底层函数可以先todo然后慢慢实现，上层要先有自己的逻辑；
* 上层协议的包头包裹在下层协议的包头里面；
* const在某些情况下很有必要，可以防止别的coder做一些错事；

## 疑问与质疑
* ARP协议处于网络层和数据链路层中间，在本代码里，ARP协议是直接操纵数据链路层的包头（ether_header），这样不好。我认为应当提供封装好的接口，这样代码可以解耦；


## 参考资料

* [网络大小端](https://www.cnblogs.com/langzou/p/9010899.html)
* [ARP报文格式](https://www.cnblogs.com/laojie4321/archive/2012/04/12/2444187.html)