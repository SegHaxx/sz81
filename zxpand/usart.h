#ifndef __USART_H
#define __USART_H

#define putrsUSART(x)

extern void serialInit(int x, int y);
extern int serialAvailable(void);
extern int serialCopy(unsigned char *);
extern void serialWrite(BYTE b);
extern int serialRead();
extern void serialHex(BYTE b);
extern void serialClose(void);
extern void Reset(void);
extern void putcUSART(char c);
extern int BusyUSART(void);

#define ASSERT_RESET

#endif

