#include <stdio.h>
#include "type.h"
#include "debug.h"
#include "uart.h"

void print_hex(uint8 *buf, uint32 size)
{	
	int32 i;	

	for(i=0;i<size;i++){		
		if((i&0xf) == 0)printf("0x%08x  ", (uint32)(buf + i));		
		printf("%02x ", buf[i]);
		if(((i+1)&0xf) == 0)printf("\n");
	}	
	printf("\n");
}

void print_hexw(uint32 *buf, uint32 size)
{	
	int32 i;	

	size = ((size + 3) & (~0x3))/2;
	for(i=0;i<size;i++){		
		if((i&0x3) == 0)printf("0x%08x  ", (uint32)(buf + i));		
		printf("%08x ", buf[i]);	
		if(((i+1)&0x3) == 0)printf("\n");
	}	
	printf("\n");
}

//==================================================
// overriding function for printf redirection -> uart
//==================================================
#undef putchar
inline int putchar(int c)
{

    /* LF -> CR+LF due to some terminal programs */
    if ( c == '\n' )
        putchar( '\r' );

    outbyte(UART_PNT_CH, c);

	return c;
}


__attribute__((used))
void nds_write(const unsigned char *buf, int size)
{	
	int    i;	
	for (i = 0; i < size; i++)		
		putchar(buf[i]);
}



