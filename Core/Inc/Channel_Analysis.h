/*
 * Channel_Analysis.h
 *  Created on: May 19, 2020
 *      Author: David
 */

#ifndef INC_CHANNEL_ANALYSIS_H_
#define INC_CHANNEL_ANALYSIS_H_


void Decompress_Channels(unsigned char *, uint8);

short CH_count;
uint32 Channel_value;

unsigned short Compression_type;
//unsigned short Channel_num;
uint32 measuredBuffer[8][16];


#endif /* INC_CHANNEL_ANALYSIS_H_ */
