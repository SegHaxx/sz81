/* W5100 Ethernet Chip emulation, but only what's needed for ZeddyNet,
   by Erik Olofsen, 2015 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <ifaddrs.h>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include "w5100.h"

// #define W_DEBUG

#define W_BUFSIZ 0x0800
#define W_BUFMSK 0x07ff

struct threadData {
       int so;
       int sn;
};

static unsigned char w_mem[0x8000];
static unsigned short w_prd[4];
static int w_ah, w_al;
static int w_sockfd[4];
static SDL_Thread* w_thread[4];
static struct threadData w_threadData[4];

extern unsigned char mem[];


uint16_t w_rn2(int addr)
{
	return (w_mem[addr] << 8) | w_mem[addr+1];
}

uint32_t w_rn4(int addr)
{
	return (w_mem[addr+3] << 24) | (w_mem[addr+2] << 16) | (w_mem[addr+1] << 8) | w_mem[addr];
}

void w_wn2(int addr, uint16_t data)
{
	w_mem[addr] = (data & 0xff00) >> 8;
	w_mem[addr+1]   = (data & 0x00ff);
}

void w_wn4(int addr, uint32_t data)
{
	w_mem[addr]   = (data & 0x000000ff);
	w_mem[addr+1] = (data & 0x0000ff00) >> 8;
	w_mem[addr+2] = (data & 0x00ff0000) >> 16;
	w_mem[addr+3] = (data & 0xff000000) >> 24;
}

void wn4(int addr, uint32_t data)
{
	mem[addr]   = (data & 0x000000ff);
	mem[addr+1] = (data & 0x0000ff00) >> 8;
	mem[addr+2] = (data & 0x00ff0000) >> 16;
	mem[addr+3] = (data & 0xff000000) >> 24;
}

void w_init()
{
	FILE *fp;
	char buf[BUFSIZ], iface[16], dest[16], gateway[16];
	int flags;
	int i;
	int a, b, c, d;
	int sd;
	struct ifreq buffer;
	unsigned int addr;
	struct ifaddrs *addrs, *tmp;
	struct sockaddr_in *paddr;

	for (i=0; i<0x8000; i++) w_mem[i] = 0;
	for (i=0; i<4; i++) w_prd[i] = 0;

// Mode IND

	w_mem[0x0000] = 0x01;

// RTR

	w_mem[0x0017] = 0x07;
	w_mem[0x0018] = 0xa0;

// RCR

	w_mem[0x0019] = 0x08;

// RMSR: default 2K for each socket

	w_mem[0x001a] = 0x55;

// TMSR: default 2K for each socket

	w_mem[0x001b] = 0x55;

	w_mem[0x420] = w_mem[0x520] = w_mem[0x620] = w_mem[0x720] = 0x08;

// PTIMER

	w_mem[0x0028] = 0x28;

// ZX81 sockets

	mem[0x3f14] = mem[0x3f15] = mem[0x3f16] = mem[0x3f17] = 0x80;

// the first iface and gateway

	if ((fp = fopen("/proc/net/route", "r")) == NULL) {
		perror("/proc/net/route");
		return;
	}
	if (!fgets(buf, sizeof(buf), fp));
	while((fscanf(fp, "%s %s %s %04d %*s %*s %*s %*s %*s %*s %*s\n", iface, dest, gateway, &flags)) != EOF) {
		if (flags!=3 || strcmp(dest,"00000000")) continue;
		sscanf(&gateway[6], "%X", &a);
		gateway[6] = 0;
		sscanf(&gateway[4], "%X", &b);
		gateway[4] = 0;
		sscanf(&gateway[2], "%X", &c);
		gateway[2] = 0;
		sscanf(gateway, "%X", &d);
		break;
	}
	fclose(fp);

// MAC

	sd = socket(PF_INET, SOCK_DGRAM, 0);
	memset(&buffer, 0x00, sizeof(buffer));
	strcpy(buffer.ifr_name, iface);
	ioctl(sd, SIOCGIFHWADDR, &buffer);
	close(sd);

	printf("- Default gateway via : %s\n", iface);
	printf("-   with MAC address  : ");
	for (i=0; i<6; i++) {
		 if (i>0) printf(":");
    		 printf("%.2x", (unsigned char)buffer.ifr_hwaddr.sa_data[i]);
		 w_mem[0x0009+i] = (unsigned char)buffer.ifr_hwaddr.sa_data[i];
	}
    	printf("\n");
	printf("- Gateway address     : %d.%d.%d.%d\n", a, b, c, d);
	w_mem[0x0001] = a;
	w_mem[0x0002] = b;
	w_mem[0x0003] = c;
	w_mem[0x0004] = d;

// DNS
	res_init();
        printf("- First DNS server at : %s\n",inet_ntoa(_res.nsaddr_list[0].sin_addr));
	addr = _res.nsaddr_list[0].sin_addr.s_addr;
	wn4(0x3f39, addr);

	getifaddrs(&addrs);
	tmp = addrs;

	while (tmp) {
	      if (tmp->ifa_addr && tmp->ifa_addr->sa_family==AF_INET &&
	          !strcmp(tmp->ifa_name,iface)) {
	              paddr = (struct sockaddr_in *)tmp->ifa_addr;
        	      printf("- Source ip address   : %s\n", inet_ntoa(paddr->sin_addr));
		      addr = paddr->sin_addr.s_addr;
		      w_wn4(0x000f, addr);
	              paddr = (struct sockaddr_in *)tmp->ifa_netmask;
        	      printf("-   with netmask      : %s\n", inet_ntoa(paddr->sin_addr));
		      addr = paddr->sin_addr.s_addr;
		      w_wn4(0x0005, addr);
	      }
	      tmp = tmp->ifa_next;
	}

// ethflags

	mem[0x3ff5] = 1;

// initialize sockets and threads

	for (i=0; i<4; i++) {
	  w_sockfd[i] = -1;
	  w_thread[i] = NULL;
	}
}

void w_exit()
{
	int retval;
	int i;

	printf("- Shutting down W5100.\n");

	for (i=0; i<4; i++) if (w_thread[i]) {
	  shutdown(w_sockfd[i], SHUT_RDWR);
	  SDL_WaitThread(w_thread[i],&retval);
        }
}

void w_sendto(int so, int sn)
{
   int n;
   unsigned int length;
   struct sockaddr_in server;
   unsigned char buf[W_BUFSIZ];
   unsigned short i, i1, i2, lbuf;

   memset(&server,0,sizeof(server));
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = w_rn4(so+Sn_DIPR0);
   server.sin_port = htons(w_rn2(so+Sn_DPORT0));
   length = sizeof(struct sockaddr_in);

   i1 = w_rn2(so+Sn_TX_RD0);
   i2 = w_rn2(so+Sn_TX_WR0);
   lbuf = i2 - i1;
   for (i=0; i<lbuf; i++) {
     i1 &= W_BUFMSK;
     buf[i] = w_mem[0x4000+sn*W_BUFSIZ+i1];
     i1++;
   }

   printf("- Exchanging via UDP  : %s", inet_ntoa(server.sin_addr));

   n = sendto(w_sockfd[sn],buf,lbuf,0,(const struct sockaddr *)&server,length);
   if (n < 0) {
     perror("w_sendto");
     return;
   }

   w_mem[so+Sn_IR] |= S_IR_SEND_OK;
   w_mem[so+Sn_TX_RD0] = w_mem[so+Sn_TX_WR0];
   w_mem[so+Sn_TX_RD1] = w_mem[so+Sn_TX_WR1];
}

int w_recvfrom(void *data)
{
   int so, sn;
   int i, i1, n;
   unsigned int length;
   struct sockaddr_in from;
   unsigned char buf[W_BUFSIZ];

   so = ((struct threadData *)data)->so;
   sn = ((struct threadData *)data)->sn;

   length = sizeof(struct sockaddr_in);

   for (;;) {

   n = recvfrom(w_sockfd[sn],buf,W_BUFSIZ,0,(struct sockaddr *)&from, &length);

   if (n < 0) {
     perror("w_recvfrom");
     return 1;
   } else if (n==0) {
#ifdef W_DEBUG
     printf("w_recvfrom: completing thread\n");
#endif
     return 0;
   }

   printf(" *\n");

   i1 = 0;

 // 8 byte (fake) header

   for (i=0; i<8; i++) {
     w_mem[0x6000+so*W_BUFSIZ+i1] = 0;
     i1++;
   }

   for (i=0; i<n; i++) {
     w_mem[0x6000+so*W_BUFSIZ+i1] = buf[i];
#ifdef W_DEBUG
     printf("w_recvfrom: %x %x %x %c\n", i, i1, buf[i], (buf[i]>32?buf[i]:32));
#endif
     i1++;
   }

   n += 8;

   w_wn2(so+Sn_RX_RSR0, n);
   w_mem[so+Sn_IR] |= S_IR_RECV;

   }

   return 0;
}

void w_send(int so, int sn)
{
   int n;
   unsigned char buf[W_BUFSIZ];
   unsigned short i, i1, i2, lbuf;

   printf("- Sending via TCP...\n");

   i1 = w_rn2(so+Sn_TX_RD0);
   i2 = w_rn2(so+Sn_TX_WR0);
   lbuf = i2 - i1;
   for (i=0; i<lbuf; i++) {
     i1 &= W_BUFMSK;
     buf[i] = w_mem[0x4000+so*W_BUFSIZ+i1];
#ifdef W_DEBUG
     printf("w_send: %x %x %c\n", i, buf[i], (buf[i]>32?buf[i]:32));
#endif
     i1++;
   }
   n = send(w_sockfd[sn],buf,lbuf,0);
   if (n < 0) {
     perror("w_send");
     return;
   }

   w_mem[so+Sn_IR] |= S_IR_SEND_OK;
   w_mem[so+Sn_TX_RD0] = w_mem[so+Sn_TX_WR0];
   w_mem[so+Sn_TX_RD1] = w_mem[so+Sn_TX_WR1];
}

int w_recv(void *data)
{
   int so, sn;
   int i, i1, n;
   unsigned char buf[W_BUFSIZ];

   so = ((struct threadData *)data)->so;
   sn = ((struct threadData *)data)->sn;

   for (;;) {

   n = recv(w_sockfd[sn],buf,W_BUFSIZ,0);

   if (n < 0) {
     perror("w_recv");
     return 1;
   } else if (n==0) {
#ifdef W_DEBUG
     printf("w_recv: completing thread\n");
#endif
     return 0;
   }

   while (w_mem[so+Sn_IR] & S_IR_RECV) {
     SDL_Delay(10);
#ifdef W_DEBUG
     printf(".");
#endif
   }

   printf("- Receiving via TCP...\n");

   i1 = w_rn2(so+Sn_RX_RD0);
   for (i=0; i<n; i++) {
     i1 &= W_BUFMSK;
     w_mem[0x6000+so*W_BUFSIZ+i1] = buf[i];
#ifdef W_DEBUG
     printf("w_recv: %x %x %x %c\n", i, i1, buf[i], (buf[i]>32?buf[i]:32));
#endif
     i1++;
   }

   w_wn2(so+Sn_RX_RSR0, n);
   w_mem[so+Sn_IR] |= S_IR_RECV;

   }

   return 0;
}

void w_connect(int so, int sn)
{
   unsigned int length;
   struct sockaddr_in server;

   memset(&server,0,sizeof(server));
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = w_rn4(so+Sn_DIPR0);
   server.sin_port = htons(w_rn2(so+Sn_DPORT0));
   length = sizeof(struct sockaddr_in);

   printf("- Connecting to       : %s\n", inet_ntoa(server.sin_addr));

   if (connect(w_sockfd[sn],(struct sockaddr *) &server,length) < 0) {
     perror("w_connect");
     w_mem[so+Sn_IR] |= S_IR_TIMEOUT;
     return;
   }

   w_mem[so+Sn_IR] |= S_IR_CON;
   w_mem[so+Sn_SR] = S_SR_SOCK_ESTABLISHED;
}

void w_socket(int addr, int val)
{
	int so, sn, sr;
	unsigned short rd, dlt;
	int retval;

	if (addr>=0x4000) {
	  w_mem[addr] = val;
	  return;
	}

	so = addr & 0x0300;
	sn = so >> 8;
	sr = addr & 0x04ff;

	switch (sr) {
	  case Sn_CR : 
#ifdef W_DEBUG
		       printf("command %x %x %x %x %x\n",addr,so,sn,sr,val);
#endif
	       	       switch(val) {
		         case S_CR_OPEN    : if (w_mem[so+Sn_MR] == S_MR_UDP) {
			      		       w_sockfd[sn] = socket(AF_INET, SOCK_DGRAM, 0);
					       w_mem[so+Sn_SR] = S_SR_SOCK_UDP;
					       w_wn2(so+Sn_RX_RD0, 0);
					       w_prd[sn] = 0;
					     } else if (w_mem[so+Sn_MR] == (S_MR_TCP|S_MR_NDMC)) {
			      		       w_sockfd[sn] = socket(AF_INET, SOCK_STREAM, 0);
					       w_mem[so+Sn_SR] = S_SR_SOCK_INIT;
					     } else {
					       printf("w5100: command not implemented...\n");
	       	           		       exit(EXIT_FAILURE);
					     }
			      		     w_mem[so+Sn_IR] = 0;
					     break;
			 case S_CR_DISCON  : printf("- Disconnecting.\n");
					     if (w_thread[sn]) {
			      		       shutdown(w_sockfd[sn], SHUT_RDWR);
					       SDL_WaitThread(w_thread[sn],&retval);
					       w_thread[sn] = NULL;
					     }
					     break;
		         case S_CR_CLOSE   : if (w_thread[sn]) {
			      		       shutdown(w_sockfd[sn], SHUT_RDWR);
					       SDL_WaitThread(w_thread[sn],&retval);
					       w_thread[sn] = NULL;
					     }
			      		     close(w_sockfd[sn]);
			      		     w_sockfd[sn] = -1;
			      		     w_mem[so+Sn_SR] = S_SR_SOCK_CLOSED;
			      		     w_mem[so+Sn_IR] &= ~S_IR_CON;
			      		     w_mem[so+Sn_IR] |= S_IR_DISCON;
			      		     w_mem[so+Sn_MR] = 0;
					     printf("- Close command done.\n");
			      		     break;
		         case S_CR_SEND    : if (w_mem[so+Sn_SR] == S_SR_SOCK_UDP) {
			      		       w_sendto(so,sn);
					       if (!w_thread[sn]) {
					         w_threadData[sn].so = so;
					         w_threadData[sn].sn = sn;
					         w_thread[sn] = SDL_CreateThread(w_recvfrom, (void *)&(w_threadData[sn]));
					       }
					     } else {
			      		       w_send(so,sn);
					       if (!w_thread[sn]) {
					         w_threadData[sn].so = so;
					         w_threadData[sn].sn = sn;
					         w_thread[sn] = SDL_CreateThread(w_recv, (void *)&(w_threadData[sn]));
					       }
					     }
			      		     break;
		         case S_CR_RECV    :
#ifdef W_DEBUG
					     printf("RECV: %x\n", w_mem[so+Sn_IR]);
#endif
			      		     rd = w_rn2(so+Sn_RX_RD0);
			      		     if (rd >= w_prd[sn])
					       dlt = rd-w_prd[sn];
					     else
					       dlt = rd+W_BUFSIZ-w_prd[sn];
					     w_prd[sn] = rd;
					     dlt = w_rn2(so+Sn_RX_RSR0)-dlt;
					     w_wn2(so+Sn_RX_RSR0, dlt);
					     if (dlt==0) {
			      		       if (w_mem[so+Sn_SR] == S_SR_SOCK_UDP) {
					         w_wn2(so+Sn_RX_RD0, 0);
						 w_prd[sn] = 0;
					       }
					       w_mem[so+Sn_IR] &= ~S_IR_RECV;
					     } else {
					       w_mem[so+Sn_IR] |= S_IR_RECV;
					     }
#ifdef W_DEBUG

					     printf("RECV: %x\n", w_mem[so+Sn_IR]);
#endif
			      		     break;
		         case S_CR_CONNECT : w_connect(so,sn);
			      		     break;
		         default           : printf("w5100: command 0x%02x not implemented...\n",val);
	       	           		     exit(EXIT_FAILURE);
		       }
	       	       break;
	  case Sn_IR : if (val==0x1f) w_mem[addr] = 0; else w_mem[addr] |= val;
	       	       break;
	  default    : w_mem[addr] = val;
	}
}

void w_write(int port, int val)
{
	unsigned int addr;
	switch (port) {
	  case WIZ_ADRH: w_ah = val;
#ifdef W_DEBUG
			 printf("*");
#endif
			 break;
	  case WIZ_ADRL: w_al = val;
#ifdef W_DEBUG
			 printf("*");
#endif
			 break;
	  case WIZ_DATA: addr = (w_ah<<8)|w_al;
	       		 if (addr==RMSR || addr==TMSR) {
			   printf("w5100: changing buffer sizes not implemented...\n");
	       	           exit(EXIT_FAILURE);
			 }
#ifdef W_DEBUG
			 printf("w_write: %x %x %c\n", addr, val, (val>32?val:32));
#endif
			 w_socket(addr,val);
			 break;
	  case WIZ_MODE: if (w_mem[MR]!=val)
			   printf("w5100: changing operation mode (from 0x%02x to 0x%02x) not implemented...\n",
			     w_mem[MR], val);
	       	         exit(EXIT_FAILURE);
	}
}

unsigned int w_read(int port)
{
	unsigned int addr;
	unsigned char val;
#ifdef W_DEBUG
	int so, sn, sr;
#endif
	if (port!=WIZ_DATA) {
	  printf("w5100: w_read port: %d\n", port);
	  exit(EXIT_FAILURE);
	}

	addr = (w_ah<<8)|w_al;
	val = w_mem[addr];

#ifdef W_DEBUG
	so = addr & 0x0300;
	sn = so >> 8;
	sr = addr & 0x04ff;

	printf("w_read: %x %x %x %x %x %x\n", addr, val, so, sn, sr, w_sockfd[sn]);
#endif
	return val;
}
