/*
 * main.c
 *
 *  Created on: Jul 6, 2018
 *      Author: adam
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/sockios.h>

///*
#define DESTMAC0 0xff
#define DESTMAC1 0xff
#define DESTMAC2 0xff
#define DESTMAC3 0xff
#define DESTMAC4 0xff
#define DESTMAC5 0xff
///*
/*
#define DESTMAC0 0x08
#define DESTMAC1 0x00
#define DESTMAC2 0x27
#define DESTMAC3 0xDE
#define DESTMAC4 0xA7
#define DESTMAC5 0xB5
*/



int main()
{
	printf("start\n");
	int raw_socket;
	if ((raw_socket = socket(AF_PACKET,SOCK_RAW,IPPROTO_RAW)) == -1)
	{
		printf("error while opening socket\n");
	}
	else printf("socket opened\n");

	struct ifreq ifreq_i;
	memset(&ifreq_i, 0, sizeof(ifreq_i));
	strncpy(ifreq_i.ifr_name,"enp0s9",IF_NAMESIZE); // set name of source interface
	printf("%s\n",ifreq_i.ifr_name);

	if((ioctl(raw_socket, SIOCGIFINDEX, &ifreq_i))<0)
		printf("error while getting index");
	printf("index = %d\n", ifreq_i.ifr_ifindex);

	struct ifreq ifreq_mac;
	memset(&ifreq_mac, 0, sizeof(ifreq_mac));
	strncpy(ifreq_mac.ifr_name,"enp0s9",IF_NAMESIZE); // set name of source interface
	printf("%s\n",ifreq_mac.ifr_name);

	if((ioctl(raw_socket, SIOCGIFHWADDR, &ifreq_mac))<0)
		printf("error while getting mac");


	unsigned char* sendbuff = (unsigned char*)malloc(1500);
	memset(sendbuff,0,1500);

	struct ethhdr *eth = (struct ethhdr *)(sendbuff);
	eth->h_source[0] = (unsigned char)(ifreq_mac.ifr_hwaddr.sa_data[0]);
	eth->h_source[1] = (unsigned char)(ifreq_mac.ifr_hwaddr.sa_data[1]);
	eth->h_source[2] = (unsigned char)(ifreq_mac.ifr_hwaddr.sa_data[2]);
	eth->h_source[3] = (unsigned char)(ifreq_mac.ifr_hwaddr.sa_data[3]);
	eth->h_source[4] = (unsigned char)(ifreq_mac.ifr_hwaddr.sa_data[4]);
	eth->h_source[5] = (unsigned char)(ifreq_mac.ifr_hwaddr.sa_data[5]);

	eth->h_dest[0] = DESTMAC0;
	eth->h_dest[1] = DESTMAC1;
	eth->h_dest[2] = DESTMAC2;
	eth->h_dest[3] = DESTMAC3;
	eth->h_dest[4] = DESTMAC4;
	eth->h_dest[5] = DESTMAC5;

	eth->h_proto = htons(0x0000); //means next header will be IP header

	/* end of ethernet header */
	int total_len = 0;
	total_len+=sizeof(struct ethhdr);

	struct sockaddr_ll sadr_ll;
	sadr_ll.sll_ifindex = ifreq_i.ifr_ifindex; // index of interface

	//unsigned long long int i=0;

	while(1)
	{
		sendto(raw_socket,sendbuff,1500,0,(const struct sockaddr*)&sadr_ll,sizeof(struct sockaddr_ll));
	}
	printf("sending packets finished");
	return 0;
}
