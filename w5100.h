// W5100/ZeddyNet definitions

// WIZ registers
#define WIZ_MODE          0x27
#define WIZ_ADRH          0x2F
#define WIZ_ADRL          0x37
#define WIZ_DATA          0x3F

// Common registers
#define MR		0x0000	// Mode register
#define GAR0		0x0001	// Gateway addr first octet
#define GAR1		0x0002
#define GAR2		0x0003
#define GAR3		0x0004
#define SUBR0		0x0005	// Subnet mask, first octet
#define SUBR1		0x0006
#define SUBR2		0x0007
#define SUBR3		0x0008
#define SHAR0		0x0009	// Our ethernet address, first of 6
#define SHAR1		0x000A
#define SHAR2		0x000B
#define SHAR3		0x000C
#define SHAR4		0x000D
#define SHAR5		0x000E
#define SIPR0		0x000F	// First octet of our IP address
#define SIPR1		0x0010
#define SIPR2		0x0011
#define SIPR3		0x0012
#define IR		0x0015	// Interrupt register
#define IMR		0x0016	// Interrupt mask
#define RTR0		0x0017	// Retry time
#define RTR1		0x0018
#define RCR		0x0019	// Retry count register
#define RMSR		0x001A	// RX memory size
#define TMSR		0x001B	// TX memory size
#define PATR0		0x001C	// PPPoE authentication type
#define PATR1		0x001D
#define PTIMER		0x0028	// PPPoE LCP request timer
#define PMAGIC		0x0029	// PPPoE LCD magic number
#define UIPR0		0x002A	// Unreachable IP address 1st octet
#define UIPR1		0x002B
#define UIPR2		0x002C
#define UIPR3		0x002D
#define UPORT0		0x002E	// Unreachable port (big endian)
#define UPORT1		0x002F

// Socket registers (base address - socket 0)
#define Sn_BASE		  	0x04	// amount to subtract from addr. to
				        // derive buffer base
#define Sn_MAX			0x08	// high byte of highest socket reg.
#define Sn_MR			0x0400	// Socket 0 mode
#define Sn_CR			0x0401	// Socket 0 command
#define Sn_IR			0x0402	// Socket 0 interrupt
#define Sn_SR			0x0403	// Socket 0 status
#define Sn_PORT0		0x0404	// Socket 0 port (big endian)
#define Sn_PORT1		0x0405
#define Sn_DHAR0		0x0406	// socket 0 destination ethernet addr.
#define Sn_DHAR1		0x0407
#define Sn_DHAR2		0x0408
#define Sn_DHAR3		0x0409
#define Sn_DHAR4		0x040A
#define Sn_DHAR5		0x040B
#define Sn_DIPR0		0x040C	// Socket 0 destination IP addr.
#define Sn_DIPR1		0x040D
#define Sn_DIPR2		0x040E
#define Sn_DIPR3		0x040F
#define Sn_DPORT0		0x0410	// Socket 0 dest port (big endian)
#define Sn_DPORT1		0x0411
#define Sn_MSSR0		0x0412	// Socket 0 max segment size
#define Sn_MSSR1		0x0413
#define Sn_PROTO		0x0414	// Socket 0 proto in IP raw mode
#define Sn_TOS			0x0415	// Type of service
#define Sn_TTL			0x0416	// Socket 0 time-to-live
#define Sn_TX_FSR0		0x0420  // Socket 0 free size
#define Sn_TX_FSR1		0x0421  // (big endian)
#define Sn_TX_RD0		0x0422	// Socket 0 TX read pointer
#define Sn_TX_RD1		0x0423
#define Sn_TX_WR0		0x0424  // Socket 0 TX write pointer
#define Sn_TX_WR1		0x0425
#define Sn_RX_RSR0		0x0426  // Socket 0 received size
#define Sn_RX_RSR1		0x0427
#define Sn_RX_RD0		0x0428  // Socket 0 RX read pointer
#define Sn_RX_RD1		0x0429
#define Sn_RX_WR0		0x042A  // Socket 0 RX write pointer
#define Sn_RX_WR1		0x042B

// Register bit values for ORing into bitfields
// Mode register
#define MR_IND			1	// Set for indirect bus IF mode
#define MR_AI 			2	// Set for address auto increment
#define MR_PPPoE 		8	// Use PPPoE
#define MR_PB			16	// Set ping block
#define MR_RST			128	// Software reset

// Interrupt register
#define IR_S0_INT		1	// Socket 0 interrupt enable
#define IR_S1_INT 		2	// Socket 1 EI
#define IR_S2_INT 		4
#define IR_S3_INT 		8
#define IR_PPPoE 		32	// PPPoE interrupts
#define IR_UNREACH 		64	// Dest unreachable interrupt
#define IR_CONFLICT 		128	// IP conflict interrupt

// Interrupt mask register - as interrupt register.
#define IM_S0_INT 	     	1
#define IM_S1_INT 		2
#define IM_S2_INT 		4
#define IM_S3_INT 		8
#define IM_PPPoE 		32
#define IM_UNREACH 		64
#define IM_CONFLICT 		128

// Socket modes
#define S_MR_CLOSED 		0
#define S_MR_TCP 		1
#define S_MR_UDP 		2
#define S_MR_IPRAW 		3
#define S_MR_MACRAW 		4
#define S_MR_PPPoE 		5
#define S_MR_NDMC 		32	// Use no delayed ACK
#define S_MR_MULTI 		128	// enable multicasting
#define S_MR_MODEMASK 		7

// Socket commands
#define S_CR_OPEN		1
#define S_CR_LISTEN		2
#define S_CR_CONNECT		4
#define S_CR_DISCON		8
#define S_CR_CLOSE		0x10
#define S_CR_SEND		0x20
#define S_CR_SEND_MAC		0x21
#define S_CR_SEND_KEEP		0x22
#define S_CR_RECV 		0x40

// Interrupt register
#define S_IR_CON		1	// Connected
#define S_IR_DISCON 		2	// Disconnected
#define S_IR_RECV 		4	// Data received
#define S_IR_TIMEOUT 		8	// Timeout occurred
#define S_IR_SEND_OK 		16	// Send completed

// poll(2) definitions (see above)
#define POLLIN			5	// S_IR_RECV | S_IR_CON
#define POLLHUP			2	// S_IR_DISCON - output only

// Bit version of the above
#define BIT_IR_CON		0
#define BIT_IR_DISCON 		1
#define BIT_IR_RECV 		2
#define BIT_IR_TIMEOUT 		3
#define BIT_IR_SEND_OK 		4

// Status register - values, not bits
#define S_SR_SOCK_CLOSED	0x00	// Socket is closed
#define S_SR_SOCK_INIT		0x13	// Socket is initialized
#define S_SR_SOCK_LISTEN	0x14	// Listen state
#define S_SR_SOCK_ESTABLISHED	0x17	// Connection establised
#define S_SR_SOCK_CLOSE_WAIT	0x1C	// CLOSE_WAIT state
#define S_SR_SOCK_UDP		0x22	// UDP socket
#define S_SR_SOCK_IPRAW		0x32	// Raw socket
#define S_SR_SOCK_MACRAW	0x42	// Raw ethernet
#define S_SR_SOCK_PPPOE		0x5F	// PPPoE socket
#define S_SR_SOCK_SYNSENT	0x15	// SYN_SENT state
#define S_SR_SOCK_SYNRECV	0x16	// SYN_RECV
#define S_SR_SOCK_FIN_WAIT	0x18	// FIN_WAIT
#define S_SR_SOCK_CLOSING	0x1A	// Closing the socket
#define S_SR_SOCK_TIME_WAIT	0x1B	// TIME_WAIT
#define S_SR_SOCK_LAST_ACK 	0x1D
#define S_SR_SOCK_ARP1		0x11
#define S_SR_SOCK_ARP2		0x21
#define S_SR_SOCK_ARP3		0x31

#define REGISTER_PAGE 		0x80	// external mem page 0
#define BUFFER_PAGE 		0x81	// external mem page 1
#define MEMMGMT			0xE0	// memory manager port

#define INVALID_SOCKNUM		4	// >4 is not valid.

void w_init();
void w_exit();
void w_write(int port, int val);
unsigned int w_read(int port);
