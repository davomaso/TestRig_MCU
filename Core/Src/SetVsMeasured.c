/*
 * SetVsMeasured.c
 *
 *  Created on: 11 Sep 2020
 *      Author: David Mason
 */
#include <main.h>
#include "SetVsMeasured.h"
#include "Global_Variables.h"
#include "Test.h"
#include "File_Handling.h"
#include "LCD.h"
#include "UART_Routine.h"

void CompareResults(TboardConfig *Board, float *SetVal) {
	uint8 ChNum = Board->latchPortCount + Board->analogInputCount + Board->digitalInputCout;
	uint8 currResult;
	uint8 spacing;
	float fMeasured;
	float comp_max;
	float comp_min;
	Tresult TresultStatus;

	Open_AppendFile(&SDcard);
	sprintf((char*)&debugTransmitBuffer[0], "\n====================	Test %d	====================\n\n", Board->GlobalTestNum + 1);
	printT((uns_ch*)&debugTransmitBuffer[0]);
	LCD_ClearLine(2);
	LCD_ClearLine(3);
	LCD_ClearLine(4);
	LCD_setCursor(2, 0);
	sprintf((char*)&debugTransmitBuffer[0], "       Test %d       ", Board->GlobalTestNum + 1);
	LCD_displayString((uns_ch*)&debugTransmitBuffer[0], strlen((char*)debugTransmitBuffer));
	LCD_ClearLine(3);
	LCD_setCursor(3, 0);

	spacing = (20 - (ChNum) - (ChNum - 1));
	spacing = (spacing & 1) ? spacing + 1 : spacing;
	spacing /= 2;
	LCD_setCursor(3, spacing);
	LCD_setCursor(3, spacing);
	for (currResult = 0; currResult < ChNum; currResult++) {
		int MeasuredVal = Board->TestResults[Board->GlobalTestNum][currResult];
		switch (Board->TestCode[currResult]) {
		case TWO_WIRE_LATCHING:
			fMeasured = (float) MeasuredVal;
			printT((uns_ch*)"Testing Latch\n");
			PortTypes[currResult] = TTLatch;
			comp_max = comp_min = 0;
			break;

		case ONE_VOLT:
		case TWOFIVE_VOLT:
		case THREE_VOLT:
			if (Board->TestCode[currResult] == ONE_VOLT)
				comp_max = comp_min = (1 * 0.01) + (0.005 * *SetVal);
			else if (Board->TestCode[currResult] == TWOFIVE_VOLT)
				comp_max = comp_min = (2.5 * 0.01) + (0.005 * *SetVal);
			else if (Board->TestCode[currResult] == THREE_VOLT)
				comp_max = comp_min = (3 * 0.01) + (0.005 * *SetVal);
			fMeasured = (float) MeasuredVal / 1000;
			printT((uns_ch*)"Testing Voltage\n");
			PortTypes[currResult] = TTVoltage;
			break;

		case TWENTY_AMP:
			fMeasured = (float) MeasuredVal / 1000;
			comp_max = comp_min = (20 * 0.005) + (0.005 * *SetVal);
			printT((uns_ch*)"Testing Currrent\n");
			PortTypes[currResult] = TTCurrent;
			break;
		case ASYNC_PULSE:
			fMeasured = (float) MeasuredVal / 1000;
			comp_max = 0; //Once this works decrease the multiplication factor to improve testing accuracy
			comp_min = 0;
			printT((uns_ch*)"Testing Async\n");
			PortTypes[currResult] = TTAsync;
			break;
		case SDI_TWELVE:
			fMeasured = (float) MeasuredVal / 1000;
			comp_max = (*SetVal); //Once this works decrease the multiplication factor to improve testing accuracy
			comp_min = (*SetVal);
			printT((uns_ch*)"Testing SDI-12\n");
			PortTypes[currResult] = TTSDI;
			break;
		case NOTEST:
			fMeasured = (float) 0;
			PortTypes[currResult] = TTNo;
			break;
		}

			//Results
		if ( ( (fMeasured <= (*SetVal + comp_max) ) && (fMeasured >= (*SetVal - comp_min) ) )
				|| (PortTypes[currResult] == TTNo) ) {
			//Pass
			sprintf((char*)&debugTransmitBuffer[0], "**Port %d** PASSED\n", currResult + 1);
			printT((uns_ch*)&debugTransmitBuffer);
			sprintf((char*)&debugTransmitBuffer[0], "Measured Value: %.03f		Set Value: %.03f \n\n", fMeasured, *SetVal); //(float)
			printT((uns_ch*)&debugTransmitBuffer);
			sprintf((char*)&debugTransmitBuffer[0], ". ");
			LCD_displayString((uns_ch*)&debugTransmitBuffer[0], strlen((char*)debugTransmitBuffer));
			TresultStatus = TRpassed;
		} else {
			//Fail
			sprintf((char*)&debugTransmitBuffer[0], "**Port %d** FAILED\n", currResult + 1);
			printT((uns_ch*)&debugTransmitBuffer);
			sprintf((char*)&debugTransmitBuffer[0], "Measured Value: %.03f		Set Value: %.03f \n\n", fMeasured, *SetVal); //(float)
			printT((uns_ch*)&debugTransmitBuffer);
			sprintf((char*)&debugTransmitBuffer[0], "X ");
			LCD_displayString((uns_ch*)&debugTransmitBuffer[0], strlen((char*)debugTransmitBuffer));
			SET_BIT(Board->TPR, (1 << Board->GlobalTestNum));
			TresultStatus = TRfailed;
		}
		//Write Results to file
		if ((currResult < Board->latchPortCount) && (PortTypes[currResult] == TTLatch)) {
			sprintf((char*)&TestResultsBuffer[0], "%x,%d,L%d,%c,%c,,,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\r\n",
					Board->BoardType, Board->GlobalTestNum + 1, (currResult + 1), PortTypes[currResult], TresultStatus,
					LatchRes.tOn, LatchRes.tOff,							//	Latch pulse width
					LatchPortA.highVoltage, LatchPortB.lowVoltage,			// 	Latch on voltage
					LatchPortB.highVoltage, LatchPortA.lowVoltage,			// 	Latch off voltage
					Vin.average, Vin.lowVoltage,							//	Vin voltages
					Vfuse.average, Vfuse.lowVoltage,						//	Vfuse voltages
					MOSFETvoltageA.highVoltage, MOSFETvoltageB.lowVoltage,	//	MOSFET on voltages
					MOSFETvoltageB.highVoltage, MOSFETvoltageA.lowVoltage	// 	MOSFET off voltages
					);
		} else if ((PortTypes[currResult] != TTNo)) {
			sprintf((char*)&TestResultsBuffer[0], "%x,%d,%d,%c,%c,%f,%f\r\n", Board->BoardType, Board->GlobalTestNum + 1,
					(currResult + 1) - Board->latchPortCount, PortTypes[currResult], TresultStatus,
					CHval[Board->GlobalTestNum][currResult], fMeasured);
		}
		Update_File(&SDcard, (char*)&SDcard.FILEname[0], (char*) &TestResultsBuffer[0]);
		SetVal++;
	}
	Close_File(&SDcard);
	if (SDcard.fresult != FR_OK) {
		uint8 SDcardRetry = 0;
		while (SDcard.fresult != FR_OK && SDcardRetry < 15) {
			HAL_Delay(100);
			Close_File(&SDcard);
			SDcardRetry++;
		}
	}

}
