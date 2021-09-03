#include "main.h"
#include "CRC16_Slow.h"

unsigned short uart_CalcCRC16(unsigned char* ptr, unsigned char count) {
	uns_ch bit;
	uint16 CRC16=0xFFFF;

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
