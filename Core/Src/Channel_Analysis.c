/*
 * Channel_Analysis.c
 *  Created on: May 19, 2020
 *      Author: David
 */
#include "main.h"
#include "Channel_Analysis.h"
#include "interogate_project.h"

void Decompress_Channels(unsigned char *data, uint8 length) {
	CH_count = 0;
	while (CH_count <= length) {
		Compression_type = (*data & 0xE0);
		Compression_type >>= 5;
//		Channel_num = (*data & 0x1F) + 1;
		switch (Compression_type) {
		case 0x00:
			data++;
			for (int i = 0; i < 4; i++) {
					Channel_value = (*data++ << 8 * i);
				}
			break;
		case 0x01:
			data++;
			for (int i = 0; i < 3; i++)
				Channel_value = (*data++ << 8 * i);
			break;
		case 0x02: //Two bytes of data when a 0x4x is returned
			data++;
			Channel_value = 0;
			for (int i = 0; i < 2; i++) {
				Channel_value |= (*data++ <<  8*i);
			}
			break;
		case 0x03:
			data++;
			Channel_value = *data++;
			break;
		case 0x04:		//Always Zero
			Channel_value = 0x0;
			data++;
			break;
		case 0x05:
			data++;
			Channel_value = 1; //Always 1
			break;
		case 0x07:		//Battery Level
			data++;
			Channel_value = *data++;
			break;
		}
		CH_count++;
		measuredBuffer[GlobalTestNum][CH_count] = Channel_value;
	}
	//UART3_transmit(&measuredBuffer[0], Channel_num);
}
