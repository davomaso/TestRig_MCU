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
	Channel_value = 0;
	uint8 Compression_type;
	uint8 length = Board->latchPortCount + Board->analogInputCount + Board->digitalInputCout;
	while (CH_count <= length+1) {
		Compression_type = 0;
		Compression_type = (*data & 0xE0);
		Compression_type >>= 5;

 		switch (Compression_type) {
		case 0x00:
			data++;
			Channel_value = 0;
			for (int i = 0; i < 4; i++) {
					Channel_value |= (*data++ << 8 * i);
				}
			break;
		case 0x01:
			data++;
			Channel_value = 0;
			for (int i = 0; i < 3; i++)
				Channel_value |= (*data++ << 8 * i);
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
			if (CH_count == length)
				Board->rawBatteryLevel[Board->GlobalTestNum] = *data++;
			else
				Channel_value = *data++;
			break;
		case 0x04:		//Always Zero
			Channel_value = 0x0;
			data++;
			break;
		case 0x05:
			data++;
			Channel_value = 1; //Always 1
		default:
			data++;
			break;

 		}
		/* if a latch result is detected do not copy the result into the result buffer, else if channel count
		 * is greater than the number of latch ports copy the result across regardless */

		if ( (( (CH_count+1) < Board->latchPortCount) && (Board->TestResults[Board->GlobalTestNum][CH_count] == 0)) || ( (CH_count+1) > Board->latchPortCount) )
			Board->TestResults[Board->GlobalTestNum][CH_count] = Channel_value;
		CH_count++;
	}
}
