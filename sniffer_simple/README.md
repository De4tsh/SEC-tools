

## sniffer_simple

为了不经过内核的协议栈，接收到所有的原始数据，所以基于原始套接字 `SOCK_RAW` 实现该 `sniffer_simple` 程序

而且为了能够捕获其数据链路层及以上所有层的数据，所以没有使用 `AF_INET` ( `AF_INET` 是面向 `IP` 层的原始套接字，对于数据链路层不可见)

而是使用 `AF_PACKET` 即面向链路层的套接字，对于 `AF_PACKET` ，其接收的数据可以为：

- `ETH_P_IP`  只接收目的 `mac` 是本机的 `IP` 类型数据帧
- `ETH_P_ARP`  只接收目的 `mac` 是本机的 `ARP` 类型数据帧
- `ETH_P_RARP`  只接收目的 `mac` 是本机的 `RARP` 类型数据帧
- `ETH_P_PAE`  只接收目的 `mac` 是本机的 `802.1x` 类型的数据帧
- `ETH_P_ALL`  接收目的 `mac` 是本机的所有类型数据帧，同时还可以接收本机发出的所有数据帧，混杂模式打开时，还可以接收到目的 `mac` 不是本机的数据帧

由于此处作为 `sniffer` 使用所以选择接受所有类型数据帧，所以选择：`ETH_P_ALL`  

至此最终的 `socket` 创建的选项就为：

```C
rawsock = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
```


![image-20221223090336041](https://raw.githubusercontent.com/De4tsh/typoraPhoto/main/img/202212230903220.png)
