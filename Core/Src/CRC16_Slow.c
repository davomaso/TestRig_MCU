#include "CRC16_Slow.h"

/*
 * Slow CRC check to check/ensure valid string is sent.
 * TODO: Update this CRC to something faster to ensure faster transmission of data
 */
unsigned short uart_CalcCRC16(unsigned char* ptr, unsigned char count) {
	unsigned char bit;
	unsigned short CRC16=0xFFFF;

	while(count--)
		{
		CRC16^=*ptr++;
		for(bit=0;bit<8;bit++)
			if(CRC16&1)
				CRC16=(CRC16>>1)^0xA001;
			else
				CRC16>>=1;
		}
	return CRC16;
	}
