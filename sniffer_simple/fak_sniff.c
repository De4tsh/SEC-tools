#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/ip_icmp.h>
#include<netinet/tcp.h>
#include<netinet/udp.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/ether.h>

#define BUFFSIZE 1024

int main()
{

	int rawsock;
	unsigned char buff[BUFFSIZE];
	int n;
	int count = 0;

	rawsock = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
	if(rawsock < 0)
	{
		printf("raw socket error!\n");
		exit(1);
	}
	while(1)
	{	
		n = recvfrom(rawsock,buff,BUFFSIZE,0,NULL,NULL);
		if(n<0)
		{
			printf("receive error!\n");
			exit(1);
		}
		
		count++;
		struct ip *ip = (struct ip*)(buff+14);
		printf("%4d	%15s",count,inet_ntoa(ip->ip_src));
		printf("%15s	%5d	%5d\n",inet_ntoa(ip->ip_dst),ip->ip_p,n);	

		int i=0,j=0;
		for(i=0;i<n;i++)
		{
			if(i!=0 && i%16==0)
			{
				printf("	");
				for(j=i-16;j<i;j++)
				{
					if(buff[j]>=32&&buff[j]<=128)
					{
						printf("%c",buff[j]);
					}
					else
					{
					       	printf(".");
					}
				}
				printf("\n");
			}
			if(i%16 == 0) 
			{
				printf("%04x	",i);
			}			
			printf("%02x",buff[i]);
	
			if(i==n-1)
			{
				for(j=0;j<15-i%16;j++) printf("  ");
				printf("	");
				for(j=i-i%16;j<=i;j++)
				{
					if(buff[j]>=32&&buff[j]<127)
					{
                               			 printf("%c",buff[j]);
					}
                               		 else printf(".");

				}
			}
		}
		printf("\n\n");

	}
}	
