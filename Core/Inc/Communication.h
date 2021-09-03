/*
 * Communication.h
 *
 *  Created on: 2 Sep 2021
 *      Author: mason
 */
#ifndef INC_COMMUNICATION_H_
#define INC_COMMUNICATION_H_

#include "main.h"
#include "Board_Config.h"

void communication_array(uns_ch Command, uns_ch * Para, uint8_t  Paralen);
void communication_response(TboardConfig * Board, uns_ch * Response, uns_ch *data, uint8 arraysize);
_Bool CRC_Check(uns_ch *input, uint8 length);
void SetPara(TboardConfig * Board, uns_ch Command);
void communication_arraySerial(uns_ch Command,uint32 CurrentSerial , uint32 NewSerial);
void switchCommsProtocol(TboardConfig *Board);

#endif /* INC_COMMUNICATION_H_ */
