/* W5100 Ethernet Chip emulation, but only what's needed for ZeddyNet,
   by Erik Olofsen, 2015-2019 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#ifdef SZ81
#include <SDL.h>
#include <SDL_thread.h>
#else
#include <pthread.h>
#endif
#include "w5100.h"

#ifdef Win32

#include <winsock2.h>
#include <iphlpapi.h>

#define SHUT_RDWR SD_BOTH

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <net/if.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>

#endif

#if defined(Win32) || defined(SIOCGIFHWADDR)

//#define W_DEBUG

#define W_BUFSIZ 0x0800
#define W_BUFMSK 0x07ff

#ifdef ZXMORE
#ifdef ZXMSHMEM
#define zxmoffset 0xf0000 /* all instances can use ZeddyNet */
#else
#define zxmoffset 0xe0000 /* for now, only the first ZX81 instance can use ZeddyNet */
#endif
#else
#define zxmoffset 0
#endif

#ifndef SZ81
extern unsigned char memory[];
#else
extern unsigned char mem[];
#endif

struct threadData {
       int so;
       int sn;
};

#ifndef SZ81

typedef struct {
	pthread_t t;
	int (*f)(void*);
	struct threadData *d;
	int r;
} SDL_Thread;

void SDL_Delay(int n)
{
	usleep(n*1000);
}

void SDL_WaitThread(SDL_Thread *thread, int *retval)
{
}

void* wrapper(void *arg)
{
	SDL_Thread *t = (SDL_Thread *) arg;

	t->r = (t->f)(t->d);
	
	return (void *) &t->r;
}

SDL_Thread* SDL_CreateThread(int (*fn)(void*), struct threadData *threaddata)
{
        SDL_Thread *tmp = malloc(sizeof(SDL_Thread));

	tmp->f = fn;
	tmp->d = threaddata;
	
	int status = pthread_create(&tmp->t, NULL, wrapper, (void *)tmp);
	if (!status) return NULL;

	return tmp;
}

#endif

static unsigned char w_mem[0x8000];
static int w_ah, w_al;
static int w_sockfd[8];
static SDL_Thread* w_thread[4];
static struct threadData w_threadData[4];

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
#ifdef SZ81
	mem[addr+zxmoffset]   = (data & 0x000000ff);
	mem[addr+zxmoffset+1] = (data & 0x0000ff00) >> 8;
	mem[addr+zxmoffset+2] = (data & 0x00ff0000) >> 16;
	mem[addr+zxmoffset+3] = (data & 0xff000000) >> 24;
#endif
}

#ifdef Win32

/* modified example code found on the web */

void w_dns()
{
#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3

#define MALLOC malloc
#define FREE free

    /* Declare and initialize variables */

	struct sockaddr_in *addr;

    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    unsigned int i = 0;

    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

    // default to unspecified address family (both)
    ULONG family = AF_INET;

    LPVOID lpMsgBuf = NULL;

    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = 0;
    ULONG Iterations = 0;

    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
    PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
    PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
    IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer = NULL;
    IP_ADAPTER_PREFIX *pPrefix = NULL;

    // Allocate a 15 KB buffer to start with.
    outBufLen = WORKING_BUFFER_SIZE;

    do {

        pAddresses = (IP_ADAPTER_ADDRESSES *) MALLOC(outBufLen);
        if (pAddresses == NULL) {
            printf
                ("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
            exit(1);
        }

        dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            FREE(pAddresses);
            pAddresses = NULL;
        } else {
            break;
        }

        Iterations++;

    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

    if (dwRetVal == NO_ERROR) {
        // If successful, output some information from the data we received
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) {

            pDnServer = pCurrAddresses->FirstDnsServerAddress;
            if (pDnServer) {
            	addr = (struct sockaddr_in *) pDnServer->Address.lpSockaddr;
				printf("- First DNS server at: %s\n",inet_ntoa(addr->sin_addr));
 				wn4(0x3f39, (uint32_t)addr->sin_addr.S_un.S_addr);
 				FREE(pAddresses);
				return;
            }

            pCurrAddresses = pCurrAddresses->Next;
        }
    } else {
        printf("Call to GetAdaptersAddresses failed with error: %d\n",
               dwRetVal);
        if (dwRetVal == ERROR_NO_DATA)
            printf("\tNo addresses were found for the requested parameters\n");
        else {

            if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                    NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),   
                    // Default language
                    (LPTSTR) & lpMsgBuf, 0, NULL)) {
                printf("\tError: %s", lpMsgBuf);
                LocalFree(lpMsgBuf);
                if (pAddresses)
                    FREE(pAddresses);
                exit(1);
            }
        }
    }

    if (pAddresses) {
        FREE(pAddresses);
    }
	
}
#endif

void w_init()
{
	FILE *fp;
	char buf[BUFSIZ], iface[16], dest[16], gateway[16];
	int flags;
	int i;
	int a, b, c, d;
	int sd;
	unsigned int addr;
	struct ifaddrs *addrs=NULL, *tmp;
	struct sockaddr_in *paddr;
#ifndef Win32
	struct ifreq buffer;
#else
	struct WSAData wsaData;
	struct hostent *phe;
	struct in_addr an_addr;
	
	WSAStartup(MAKEWORD(1,1), &wsaData);
#endif

#ifdef SZ81
//	for (i=0x2000; i<0x4000; i++) mem[zxmoffset+i] = 0;
//	for (i=0x3000; i<0x3400; i++) mem[zxmoffset+i] = 0xaa;
#endif
	for (i=0; i<0x8000; i++) w_mem[i] = 0;

// Set the nonzero defaults, even if they are not used in the present emulation
	
// RTR

	w_mem[RTR0] = 0x07;
	w_mem[RTR1] = 0xd0;

// RCR

	w_mem[RCR] = 0x08;

// RMSR: default 2K for each socket (changing it not implemented)

	w_mem[RMSR] = 0x55;

// TMSR: default 2K for each socket (changing it not implemented)

	w_mem[TMSR] = 0x55;

// TX free size

	w_mem[0x420] = w_mem[0x520] = w_mem[0x620] = w_mem[0x720] = 0x08;

// PTIMER

	w_mem[PTIMER] = 0x28;

// ZX81 sockets
#ifdef SZ81
//	mem[0x3f14+zxmoffset] = mem[0x3f15+zxmoffset] = mem[0x3f16+zxmoffset] = mem[0x3f17+zxmoffset] = 0x80;
#endif

#ifndef Win32

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
	
	freeifaddrs(addrs);

#else

w_dns();

#endif

// ethflags
#ifdef SZ81
	mem[0x3ff5+zxmoffset] = 1;
#endif
// initialize sockets and threads

	for (i=0; i<4; i++) {
	  w_sockfd[i] = w_sockfd[4+i] = 0;
	  w_thread[i] = NULL;
	}
}

void w_shutdown_fd(int fd) {
#ifdef W_DEBUG
	printf("w_shutdown_fd: %x\n", fd);
#endif
#ifndef Win32
	shutdown(fd, SHUT_RDWR);
#else
	closesocket(fd);
#endif
}

void w_shutdown_thread(int i) {
	int retval;

	SDL_WaitThread(w_thread[i],&retval);
	w_thread[i] = NULL;
#ifdef W_DEBUG
	printf("w_shutdown_thread: %d %d\n", i, retval);
#endif
}

void w_shutdown(int i)
{
#ifdef W_DEBUG
	printf("w_shutdown: %d %x %x %lx\n", i, w_sockfd[i], w_sockfd[4+i], (unsigned long)w_thread[i]);
#endif
	if (w_sockfd[i]) {
	  if (w_sockfd[4+i]) {
	      w_shutdown_fd(w_sockfd[4+i]);
	      w_sockfd[4+i] = 0;
	  }
	  w_shutdown_fd(w_sockfd[i]);
	  w_sockfd[i] = 0;
	  if (w_thread[i]) w_shutdown_thread(i);
        }
}

void w_reset()
{
	int i;

	printf("- Resetting W5100.\n");

	for (i=0; i<4; i++) w_shutdown(i);

#ifdef Win32
	printf("- WSACleanup().\n");
	WSACleanup();
#endif

	w_init();
}

void w_exit()
{
	int i;

	printf("- Shutting down W5100.\n");

	for (i=0; i<4; i++) w_shutdown(i);

#ifdef Win32
	printf("- WSACleanup().\n");
	WSACleanup();
#endif
}

void w_sendto(int so, int sn)
{
   unsigned int length;
   struct sockaddr_in server;
   unsigned char buf[W_BUFSIZ];
   int n, i, i1, i2, lbuf;

   memset(&server,0,sizeof(server));
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = w_rn4(so+Sn_DIPR0);
   server.sin_port = htons(w_rn2(so+Sn_DPORT0));
   length = sizeof(struct sockaddr_in);

   i1 = w_rn2(so+Sn_TX_RD0) & W_BUFMSK;
   i2 = w_rn2(so+Sn_TX_WR0) & W_BUFMSK;
   lbuf = i2 - i1;
   if (lbuf < 0) lbuf += W_BUFSIZ;

   for (i=0; i<lbuf; i++) {
     i1 &= W_BUFMSK;
     buf[i] = w_mem[0x4000+sn*W_BUFSIZ+i1];
     i1++;
   }

   if (w_mem[so+Sn_MR] == S_MR_UDP)
     printf("- Exchanging via UDP  : %s", inet_ntoa(server.sin_addr));
   else
     printf("- Exchanging via RAW  : %s", inet_ntoa(server.sin_addr));

   n = sendto(w_sockfd[sn],buf,lbuf,0,(const struct sockaddr *)&server,length);
   if (n < 0) {
     perror("w_sendto");
     return;
   }

   w_mem[so+Sn_IR] |= S_IR_SEND_OK;
   w_mem[IR] |= (1<<sn);

   w_mem[so+Sn_TX_RD0] = w_mem[so+Sn_TX_WR0];
   w_mem[so+Sn_TX_RD1] = w_mem[so+Sn_TX_WR1];
}

int w_recvfrom(void *data)
{
   int so, sn;
   int i, ioff, i1, n;
   unsigned int length;
   struct sockaddr_in from;
   unsigned char buf[W_BUFSIZ];

   so = ((struct threadData *)data)->so;
   sn = ((struct threadData *)data)->sn;

   length = sizeof(struct sockaddr_in);

   for (;;) {

   /* -8 for the fake header */

   n = recvfrom(w_sockfd[sn],buf,W_BUFSIZ-8,0,(struct sockaddr *)&from, &length);

   if (n < 0) {
     perror("w_recvfrom");
     return 1;
   } else if (n==0) {
     printf("- Connection closed\n");
     w_mem[so+Sn_IR] |= S_IR_DISCON;
     w_mem[IR] |= (1<<sn);
     w_mem[so+Sn_SR] = S_SR_SOCK_CLOSE_WAIT;
#ifdef W_DEBUG
     printf("w_recvfrom: completing thread\n");
#endif
//     w_thread[sn] = NULL;
     return 0;
   }

   printf(" *\n");

   i1 = w_rn2(so+Sn_RX_WR0);

   ioff = 0;

   if (w_mem[so+Sn_MR] == S_MR_UDP) {
 // 8 byte (fake) header

     for (i=0; i<8; i++) {
       i1 &= W_BUFMSK;
       w_mem[0x6000+sn*W_BUFSIZ+i1] = 0;
       i1++;
     }

   } else if (w_mem[so+Sn_MR] == S_MR_IPRAW) {
 // change 20 byte to 6 byte header header

     for (i=0; i<4; i++) {
       i1 &= W_BUFMSK;
       w_mem[0x6000+sn*W_BUFSIZ+i1] = buf[12+i];
       i1++;
     }

     n -= 20;
     ioff = 20;

     i1 &= W_BUFMSK;
     w_mem[0x6000+sn*W_BUFSIZ+i1] = (n & 0xff00) >> 8;
     i1++;
     i1 &= W_BUFMSK;
     w_mem[0x6000+sn*W_BUFSIZ+i1] = (n & 0x00ff);
     i1++;

   }

   for (i=0; i<n; i++) {
     i1 &= W_BUFMSK;
     w_mem[0x6000+sn*W_BUFSIZ+i1] = buf[i+ioff];
#ifdef W_DEBUG
     printf("w_recvfrom: %x %x %x %c\n", i, i1, buf[i+ioff], (buf[i+ioff]>32?buf[i+ioff]:32));
#endif
     i1++;
   }
   w_wn2(so+Sn_RX_WR0, i1);

   if (w_mem[so+Sn_MR] == S_MR_UDP) n += 8;
   else if (w_mem[so+Sn_MR] == S_MR_IPRAW) n += 6;

   n += w_rn2(so+Sn_RX_RSR0);
   w_wn2(so+Sn_RX_RSR0, n);
   w_mem[so+Sn_IR] |= S_IR_RECV;
   w_mem[IR] |= (1<<sn);
   }

   return 0;
}

void w_send(int so, int sn)
{
   unsigned char buf[W_BUFSIZ];
   int n, i, i1, i2, lbuf;

   printf("- Sending via TCP...\n");

   i1 = w_rn2(so+Sn_TX_RD0) & W_BUFMSK;
   i2 = w_rn2(so+Sn_TX_WR0) & W_BUFMSK;
   lbuf = i2 - i1;
   if (lbuf < 0) lbuf += W_BUFSIZ;

   for (i=0; i<lbuf; i++) {
     i1 &= W_BUFMSK;
     buf[i] = w_mem[0x4000+sn*W_BUFSIZ+i1];
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
   w_mem[IR] |= (1<<sn);

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
     printf("- Connection closed\n");
     w_mem[so+Sn_IR] |= S_IR_DISCON;
     w_mem[IR] |= (1<<sn);
     w_mem[so+Sn_SR] = S_SR_SOCK_CLOSE_WAIT;
#ifdef W_DEBUG
     printf("w_recv: completing thread\n");
#endif
//     w_thread[sn] = NULL;
     return 0;
   }

   printf("- Receiving via TCP...\n");

   i1 = w_rn2(so+Sn_RX_WR0);
   for (i=0; i<n; i++) {
     i1 &= W_BUFMSK;
     w_mem[0x6000+sn*W_BUFSIZ+i1] = buf[i];
#ifdef W_DEBUG
     printf("w_recv: %x %x %x %c\n", i, i1, buf[i], (buf[i]>32?buf[i]:32));
#endif
     i1++;
   }
   w_wn2(so+Sn_RX_WR0, i1);

   n += w_rn2(so+Sn_RX_RSR0);
   w_wn2(so+Sn_RX_RSR0, n);
   w_mem[so+Sn_IR] |= S_IR_RECV;
   w_mem[IR] |= (1<<sn);
   }

   return 0;
}

int w_bind(int so, int sn)
{
   struct sockaddr_in tmp_addr;

   tmp_addr.sin_family = AF_INET;
   tmp_addr.sin_addr.s_addr = INADDR_ANY;
   tmp_addr.sin_port = htons(w_rn2(so+Sn_PORT0));

   return bind(w_sockfd[sn], (struct sockaddr*)&tmp_addr, sizeof(tmp_addr));
}

int w_accept(void *data)
{
   int so, sn, connfd;
   struct sockaddr_in their_addr;
   unsigned int addrsz = sizeof(struct sockaddr_in);
   
   so = ((struct threadData *)data)->so;
   sn = ((struct threadData *)data)->sn;

   printf("- Waiting for connection...\n");

   connfd = accept(w_sockfd[sn], (struct sockaddr*)&their_addr, &addrsz);

   if(connfd < 0) {
	perror("w_accept");
	return connfd;
   } else {
        printf("- Connection accepted.\n");
   }

   w_wn2(so+Sn_DPORT0, ntohs(their_addr.sin_port));
   w_wn4(so+Sn_DIPR0, their_addr.sin_addr.s_addr);
	
   w_mem[so+Sn_IR] |= S_IR_CON;
   w_mem[IR] |= (1<<sn);
   w_mem[so+Sn_SR] = S_SR_SOCK_ESTABLISHED;

   w_sockfd[4+sn] = w_sockfd[sn];
   w_sockfd[sn] = connfd;

   return w_recv(data);
}

int w_connect(int so, int sn)
{
   unsigned int length;
   struct sockaddr_in server;
   int status;

   memset(&server,0,sizeof(server));
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = w_rn4(so+Sn_DIPR0);
   server.sin_port = htons(w_rn2(so+Sn_DPORT0));
   length = sizeof(struct sockaddr_in);

   printf("- Connecting to       : %s\n", inet_ntoa(server.sin_addr));

   status = connect(w_sockfd[sn],(struct sockaddr *) &server,length);
   
   if (status < 0) {
     perror("w_connect");
     w_mem[so+Sn_IR] |= S_IR_TIMEOUT;    // p.49
     w_mem[IR] |= (1<<sn);
     w_mem[so+Sn_SR] = S_SR_SOCK_CLOSED; // p.49->47: the CLOSE command is still needed
     printf("- Timed out.\n");
   } else {
     w_mem[so+Sn_IR] |= S_IR_CON;        // p.49
     w_mem[IR] |= (1<<sn);
     w_mem[so+Sn_SR] = S_SR_SOCK_ESTABLISHED;
     printf("- Connection established.\n");
   }

   return status;
}

/* socket operations */

void w_op_close(int so, int sn)
{
	printf("- Closing socket %d.\n",sn);
	w_shutdown(sn);
	w_mem[so+Sn_SR] = S_SR_SOCK_CLOSED; // p.27, DISCON vs. CLOSE
}

void w_op_discon(int so, int sn)
{
	if (w_mem[so+Sn_SR]!=S_SR_SOCK_ESTABLISHED) {
		printf("w5100: socket not in established state.\n");
	}
	w_op_close(so, sn);
}

void w_op_open(int so, int sn)
{
	int enable = 1;

	if (w_mem[so+Sn_SR]!=S_SR_SOCK_CLOSED) w_op_close(so, sn);

	if ((w_mem[so+Sn_MR] & S_MR_MODEMASK) == S_MR_UDP) {
		if ((w_sockfd[sn] = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("w5100: S_CR_OPEN failed");
		} else {
			w_mem[so+Sn_SR] = S_SR_SOCK_UDP;
		}
	} else if ((w_mem[so+Sn_MR] & S_MR_MODEMASK) == S_MR_TCP) {
		if ((w_sockfd[sn] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("w5100: S_CR_OPEN failed");
		 } else {
			if (setsockopt(w_sockfd[sn], SOL_SOCKET, SO_REUSEADDR, (void *)&enable, sizeof(int)) < 0) {
				perror("w5100: setsockopt(SO_REUSEADDR) failed");
			}
			w_mem[so+Sn_SR] = S_SR_SOCK_INIT;
		}
	} else if ((w_mem[so+Sn_MR] & S_MR_MODEMASK) == S_MR_IPRAW) {
		if ((w_sockfd[sn] = socket(AF_INET, SOCK_RAW, w_mem[so+Sn_PROTO])) < 0) {
			perror("w5100: S_CR_OPEN failed");
		} else {
			w_mem[so+Sn_SR] = S_SR_SOCK_IPRAW;
		}
	} else {
		printf("w5100: w_op_open: mode not supported.\n");
	       	exit(EXIT_FAILURE);
	}

	if (w_mem[so+Sn_SR]!=S_SR_SOCK_INIT && !w_thread[sn]) {
		w_threadData[sn].so = so;
		w_threadData[sn].sn = sn;
#ifdef SDL2
		w_thread[sn] = SDL_CreateThread(w_recvfrom, NULL, (void *)&(w_threadData[sn]));
#else
		w_thread[sn] = SDL_CreateThread(w_recvfrom, (void *)&(w_threadData[sn]));
#endif
	}
	
	printf("- Socket %d opened.\n",sn);
}

void w_op_listen(int so, int sn)
{
	if (w_mem[so+Sn_SR]==S_SR_SOCK_INIT) {
		if (w_bind(so,sn)) {
			perror("w5100: bind");
	       	        exit(EXIT_FAILURE);
		}
		if (!listen(w_sockfd[sn],0)) {
			w_mem[so+Sn_SR] = S_SR_SOCK_LISTEN;
		} else {
			perror("w5100: listen");
			exit(EXIT_FAILURE);
		}
	} else {
		printf("w5100: socket not in initialized state.\n");
	       	exit(EXIT_FAILURE);
	}
	
	if (!w_thread[sn]) {
		w_threadData[sn].so = so;
		w_threadData[sn].sn = sn;
#ifdef SDL2
		w_thread[sn] = SDL_CreateThread(w_accept, NULL, (void *)&(w_threadData[sn]));
#else
		w_thread[sn] = SDL_CreateThread(w_accept, (void *)&(w_threadData[sn]));
#endif
	}

	printf("- Listening...\n");
}

void w_op_connect(int so, int sn)
{
	int status;

	if (w_mem[so+Sn_SR]!=S_SR_SOCK_INIT) {
		printf("w5100: socket not in initialized state.\n");
	} else {
// status SOCK_SYNSENT not possible with the present routine
		status = w_connect(so,sn);
		if (status < 0) return;
		w_threadData[sn].so = so;
		w_threadData[sn].sn = sn;
#ifdef SDL2
		w_thread[sn] = SDL_CreateThread(w_recv, NULL, (void *)&(w_threadData[sn]));
#else
		w_thread[sn] = SDL_CreateThread(w_recv, (void *)&(w_threadData[sn]));
#endif
	}
}

void w_op_send(int so, int sn)
{
	if (w_mem[so+Sn_SR]==S_SR_SOCK_UDP || w_mem[so+Sn_SR]==S_SR_SOCK_IPRAW) {
		w_sendto(so,sn);
	} else if (w_mem[so+Sn_SR]==S_SR_SOCK_ESTABLISHED) {
		w_send(so,sn);
	} else {
		printf("w5100: socket not opened.\n");
	}
}

void w_op_recv(int so, int sn)
{
	int nw, nr, dlt;
#ifdef W_DEBUG
	printf("RECV: %x\n", w_mem[so+Sn_IR]);
#endif
	nw = w_rn2(so+Sn_RX_WR0) & W_BUFMSK;
	nr = w_rn2(so+Sn_RX_RD0) & W_BUFMSK;
	dlt = nw - nr;
	if (dlt < 0) dlt += W_BUFSIZ;
	w_wn2(so+Sn_RX_RSR0, dlt);

	if (dlt) {
		w_mem[so+Sn_IR] |= S_IR_RECV;
		w_mem[IR] |= (1<<sn);
	}
}

void w_socket_ops(int addr, int val)
{
	int so, sn, sr;

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
		       printf("\ncommand %x %x %x %x %x\n",addr,so,sn,sr,val);
#endif
	       	       switch(val) {
		         case S_CR_OPEN    : w_op_open(so, sn);
					     break;
		         case S_CR_LISTEN  : w_op_listen(so, sn);
					     break;
		         case S_CR_CONNECT : w_op_connect(so, sn);
					     break;
			 case S_CR_DISCON  : w_op_discon(so, sn);
					     // break;
		         case S_CR_CLOSE   : w_op_close(so, sn);
			      		     break;
		         case S_CR_SEND    : w_op_send(so, sn);
			      		     break;
		         case S_CR_RECV    : w_op_recv(so, sn);
			      		     break;
		         default           : printf("w5100: command 0x%02x not implemented.\n",val);
				             exit(EXIT_FAILURE);
		       }
	       	       break;

	  case Sn_IR : w_mem[addr] &= ~val; // writing a 1 clears the bit
		       if (!w_mem[addr]) w_mem[IR] &= ~(1<<sn);
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
	  		 if (addr==0 && val&0x80) {
				 w_reset();
				 break;
		         } else if (w_mem[MR] & 0x02) {
				 addr++;
				 w_ah = (addr & 0xff00) >> 8;
				 w_al = addr & 0x00ff;
				 addr--;
			 }
	       		 if ((addr==RMSR || addr==TMSR) && (val != w_mem[addr])) {
				 printf("w5100: buffer sizes other than default not implemented.\n");
				 exit(EXIT_FAILURE);
			 }
#ifdef W_DEBUG
			 printf("w_write: %x %x %c\n", addr, val, (val>32?val:32));
#endif
			 w_socket_ops(addr,val);
			 break;
	  case WIZ_MODE: if (val&0x80) {
				 w_reset();
				 return;
			 }
			 w_mem[MR] = val;
	}
}

unsigned int w_read(int port)
{
	unsigned int addr;
	unsigned char val;
#ifdef W_DEBUG
	int so, sn, sr;
	static unsigned int oldval=-1;
	static int oldport=-1;
#endif
	if (port==WIZ_MODE) {
		printf("w5100: w_read mode: %d\n", w_mem[0]);
		return w_mem[0];
	}

	if (port!=WIZ_DATA) {
		printf("w5100: w_read port: %d\n", port);
		exit(EXIT_FAILURE);
	}

	addr = (w_ah<<8)|w_al;
	val = w_mem[addr];

	if (w_mem[MR] & 0x02) {
		addr++;
		w_ah = (addr & 0xff00) >> 8;
		w_al = addr & 0x00ff;
		addr--;
	}

#ifdef W_DEBUG
	so = addr & 0x0300;
	sn = so >> 8;
	sr = addr & 0x04ff;

	if (val!=oldval || port!=oldport) {
	oldval = val;
	oldport = port;
	printf("w_read: %x %x %x %x %x %x\n", addr, val, so, sn, sr, w_sockfd[sn]);
	}
#endif
	
	return val;
}

#else
// Mac OS X network functions will go here, for now networking is not supported
// on Mac OS X due to no definition for SIOCGIFHWADDR and no /proc file system.
void w_init()
{
}
void w_exit()
{
}
unsigned int w_read()
{
	return(0);
}
void w_write()
{
}
#endif
