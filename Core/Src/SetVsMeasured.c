/*
 * SetVsMeasured.c
 *
 *  Created on: 11 Sep 2020
 *      Author: David Mason
 */
#include <main.h>
#include "SetVsMeasured.h"
#include "interogate_project.h"
#include "Test.h"
#include "SDcard.h"

void CompareResults(TboardConfig * Board, float *SetVal)
{
	uint8 ChNum = Board->latchPortCount + Board->analogInputCount + Board->digitalInputCout;
	uint8 currResult;
	uint8 spacing;
	float fMeasured;
	float comp_max;
	float comp_min;
	Tresult TresultStatus;

	sprintf(debugTransmitBuffer,"\n====================	Test %d	====================\n\n",Board->GlobalTestNum+1);
	CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
	  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);

	LCD_setCursor(2, 0);
	sprintf(debugTransmitBuffer, "       Test %d       ", Board->GlobalTestNum+1);
	LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
	LCD_ClearLine(3);
	LCD_setCursor(3, 0);

	spacing = (20 - (ChNum) - (ChNum-1));
	spacing = (spacing & 1) ? spacing+1 : spacing;
	spacing /= 2;
	LCD_setCursor(3, spacing);
 	LCD_setCursor(3, spacing);

 	if (Board->GlobalTestNum == 0) {
		sprintf(FILEname, "/TEST_RESULTS/%d.CSV",Board->SerialNumber);
		sprintf(debugTransmitBuffer, "Board,Test,Port,TestType,Pass/Fail,Set,Measured, ton, toff, V1h, V2l, V2h, V1l, VinAVG, VinLow, VfuseAVG, VfuseLow, MOSonHigh, MOSonLow, MOSoffHigh, MOSoffLow\r\n");
		if ( Write_File(&FILEname[0], &debugTransmitBuffer[0]) != FR_OK) {
			Mount_SD("/");
			Write_File(&FILEname[0], &debugTransmitBuffer[0]);
		}
 	}

	for (currResult = 0;currResult < ChNum;currResult++) {
		int MeasuredVal = Board->TestResults[Board->GlobalTestNum][currResult];
		switch (Board->TestCode[currResult]) {
		case TWO_WIRE_LATCHING:
			fMeasured = (float)MeasuredVal;
			printT("Testing Latch\n");
			PortTypes[currResult] = TTLatch;
			comp_max = comp_min = 0;
			break;

		case ONE_VOLT:
		case TWOFIVE_VOLT:
		case THREE_VOLT:
			if (Board->TestCode[currResult] == ONE_VOLT)
				comp_max = comp_min = (1*0.01) + (0.005 * *SetVal);
			else if (Board->TestCode[currResult] == TWOFIVE_VOLT)
				comp_max = comp_min = (2.5*0.01) + (0.005 * *SetVal);
			else if (Board->TestCode[currResult] == THREE_VOLT)
				comp_max = comp_min = (3*0.01) + (0.005 * *SetVal);
			fMeasured = (float) MeasuredVal / 1000;
			sprintf(debugTransmitBuffer,"Testing Voltage\n");
			CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			PortTypes[currResult] = TTVoltage;
			break;

		case TWENTY_AMP:
			fMeasured = (float) MeasuredVal / 1000;
			comp_max = comp_min = (20*0.005) + (0.005 * *SetVal);
			sprintf(debugTransmitBuffer,"Testing Currrent\n");
			CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			PortTypes[currResult] = TTCurrent;
			break;
		case ASYNC_PULSE:
			fMeasured = (float) MeasuredVal / 1000;
			comp_max = 0;//Once this works decrease the multiplication factor to improve testing accuracy
			comp_min = 0;
			sprintf(debugTransmitBuffer,"Testing Async\n");
			CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			PortTypes[currResult] = TTAsync;
			break;
		case SDI_TWELVE:
			fMeasured = (float) MeasuredVal / 1000;
			comp_max = (*SetVal); //Once this works decrease the multiplication factor to improve testing accuracy
			comp_min = (*SetVal);
			sprintf(debugTransmitBuffer,"Testing SDI-12\n");
			CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			PortTypes[currResult] = TTSDI;
			break;
		case NOTEST:
			fMeasured = (float)0;
			MeasuredVal;
			PortTypes[currResult] = TTNo;
			break;
		}
		if( (fMeasured <= (*SetVal + comp_max)) && (fMeasured >= (*SetVal - comp_min)) || PortTypes[currResult] == TTNo )
		{
			//Pass
			sprintf(debugTransmitBuffer,"**Port %d** PASSED\n",currResult+1);
			CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			sprintf(debugTransmitBuffer,"Measured Value: %.03f		Set Value: %.03f \n\n",fMeasured , *SetVal); //(float)
			CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			  HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			sprintf(debugTransmitBuffer, ". ");
			LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			TresultStatus = TRpassed;
		}
		else
		{
			//Fail
			sprintf(debugTransmitBuffer,"**Port %d** FAILED\n",currResult+1);
			CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			sprintf(debugTransmitBuffer,"Measured Value: %.03f		Set Value: %.03f \n\n",fMeasured , *SetVal); //(float)
			CDC_Transmit_FS(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			HAL_UART_Transmit(&huart1, &debugTransmitBuffer[0], strlen(debugTransmitBuffer), HAL_MAX_DELAY);
			sprintf(debugTransmitBuffer, "X ");
			LCD_printf(&debugTransmitBuffer[0], strlen(debugTransmitBuffer));
			CLEAR_BIT(Board->TPR, (1 <<Board->GlobalTestNum) );
			TresultStatus = TRfailed;
		}
			//Write Results to file
		if ((currResult < Board->latchPortCount) && (PortTypes[currResult] == TTLatch)) {
			sprintf(debugTransmitBuffer,
					"%d,%d,L%d,%c,%c,,,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\r\n",
					Board->BoardType, Board->GlobalTestNum,
					(currResult + 1),
					PortTypes[currResult], TresultStatus, LatchRes.tOn,
					LatchRes.tOff, LatchRes.PortAhighVoltage,
					LatchRes.PortBlowVoltage, LatchRes.PortBhighVoltage,
					LatchRes.PortAlowVoltage, LatchRes.InAvgVoltage,
					LatchRes.InLowVoltage, LatchRes.FuseAvgVoltage,
					LatchRes.FuseLowVoltage, LatchRes.MOSonHigh,
					LatchRes.MOSonLow, LatchRes.MOSoffHigh, LatchRes.MOSoffLow);
		} else if ((PortTypes[currResult] != TTNo)){
			sprintf(debugTransmitBuffer, "%d,%d,%d,%,%c,%f,%f\r\n", Board->BoardType, Board->GlobalTestNum, (currResult+1)-Board->latchPortCount, PortTypes[currResult], TresultStatus,*SetVal,fMeasured);
		}
		Open_AppendFile(&FILEname[0]);
		Update_File(&FILEname[0], debugTransmitBuffer);
		*SetVal++;
	}
//	Close_File(&FILEname[0]);
//	WriteSDresults(&PortTypes[0] ,&SetResults[0], &MeasuredResults[0]);
	Board->GlobalTestNum++;
}
