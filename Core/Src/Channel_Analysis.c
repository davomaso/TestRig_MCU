/*
 * Channel_Analysis.c
 *  Created on: May 19, 2020
 *      Author: David
 */
#include "main.h"
#include "Global_Variables.h"
#include "Channel_Analysis.h"

void Decompress_Channels(TboardConfig * Board, uns_ch *data) {
	uint8 CH_count = 0;
	uint32 Channel_value;
	uint8 Channel;
	Channel_value = 0;
	uint8 Compression_type;
	uint8 length = Board->ChCount;	//	Acount for battery voltage
	if (Board->BoardType == b402x)	// Increase length by 1 on 4020 to ensure battery level is read
		length += 1;				// Accounting for the output port
	while (CH_count <= length) {	// Inclusive to acount for the battery level voltage
		Compression_type = 0;
		Compression_type = (*data & 0xE0);
		Compression_type >>= 5;
		Channel = *data & 0x1F;

 		switch (Compression_type) {
		case 0x00:
			Channel_value = 0;
			for (int i = 0; i < 4; i++) {
					Channel_value |= (*data++ << 8 * i);
				}
			break;
		case 0x01:						// 3 Bytes of data
			data++;
			Channel_value = 0;
			for (int i = 0; i < 3; i++)
				Channel_value |= (*data++ << 8 * i);
			break;
		case 0x02: 						//Two bytes of data when a 0x4x is returned
			data++;
			Channel_value = 0;
			for (int i = 0; i < 2; i++) {
				Channel_value |= (*data++ <<  8*i);
			}
			break;
		case 0x03:						// 1 byte of data
			data++;
			Channel_value = *data++;
			break;
		case 0x04:						//Always Zero
			Channel_value = 0x0;
			data++;
			break;
		case 0x05:						//Always 1
			data++;
			Channel_value = 1;
		default:						// default case
			data++;
			break;

 		}
		/* if a latch result is detected do not copy the result into the result buffer, else if channel count
		 * is greater than the number of latch ports copy the result across regardless */

		if ( Channel >= (Board->latchPortCount * 2)  )
			Board->TestResults[Board->GlobalTestNum][Channel] = Channel_value;
		CH_count++;
	}
}
