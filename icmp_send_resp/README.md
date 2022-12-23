## 套接字

```C
sockfd = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
```

## ICMP 结构体

```C
struct icmp_ra_addr
{
  uint32_t ira_addr;
  uint32_t ira_preference;
};

struct icmp
{
  uint8_t  icmp_type;   /* type of message, see below */
  uint8_t  icmp_code;   /* type sub code */
  uint16_t icmp_cksum;  /* ones complement checksum of struct */
  union
  {
    unsigned char ih_pptr;      /* ICMP_PARAMPROB */
    struct in_addr ih_gwaddr;   /* gateway address */
    struct ih_idseq             /* echo datagram */
    {
      uint16_t icd_id;
      uint16_t icd_seq;
    } ih_idseq;
    uint32_t ih_void;

    /* ICMP_UNREACH_NEEDFRAG -- Path MTU Discovery (RFC1191) */
    struct ih_pmtu
    {
      uint16_t ipm_void;
      uint16_t ipm_nextmtu;
    } ih_pmtu;

    struct ih_rtradv
    {
      uint8_t irt_num_addrs;
      uint8_t irt_wpa;
      uint16_t irt_lifetime;
    } ih_rtradv;
  } icmp_hun;

```

因为不同的 `ICMP` 数据包中内容不同，所以有不少的联合体，为了好用，专门重定义：

```C
#define icmp_id         icmp_hun.ih_idseq.icd_id
#define icmp_seq        icmp_hun.ih_idseq.icd_seq
```

所以对于 ping request 的构造，所需的字段如下

```C
icmp = (struct icmp*)sendbuff;
        icmp->icmp_type = ICMP_ECHO;
        icmp->icmp_code = 0;
        icmp->icmp_cksum = 0;
        icmp->icmp_id=2;
        icmp->icmp_seq=3;
```

其中 `id` `seq` 等值无需固定，更灵活的使用在 `nmap` 源码中有更多的体现
