#include "integer.h"

void serialInit(int x, int y) {}
int serialAvailable(void) { return 0; }
int serialCopy(unsigned char *) { return 0; }
void serialWrite(BYTE b) {}
int serialRead() { return 0; }
void serialHex(BYTE b) {}
void serialClose(void) {}
void Reset(void) {}
void putcUSART(char c) {}
int BusyUSART(void) { return 0; }
