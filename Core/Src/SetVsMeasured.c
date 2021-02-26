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

void CompareResults(int * MeasuredVal, float *SetVal,uint8  ChNum)
{
	sprintf(Buffer,"\n====================	Test %d	====================\n\n",GlobalTestNum);
	CDC_Transmit_FS(&Buffer[0], strlen(Buffer));

	LCD_setCursor(2, 0);
	sprintf(Buffer, "       Test %d       ", GlobalTestNum);
	LCD_printf(&Buffer[0], strlen(Buffer));

	LCD_ClearLine(3);
	LCD_setCursor(3, 0);
	uint8 spacing = (20 - (ChNum) - (ChNum-1));
	spacing = (spacing & 1) ? spacing+1 : spacing;
	spacing /= 2;
	LCD_setCursor(3, spacing);
 	LCD_setCursor(3, spacing);
	sprintf(FILEname, "/TEST_RESULTS/%d.CSV",BoardConnected.SerialNumber);
	Create_File(&FILEname[0]);
	sprintf(Buffer, "Board,Test,Port,TestType,Pass/Fail,Set,Measured, ton, toff, V1h, V2l, V2h, V1l, VinAVG, VinLow, VfuseAVG, VfuseLow, MOSonHigh, MOSonLow, MOSoffHigh, MOSoffLow\r\n");
	Write_File(&FILEname[0], &Buffer[0]);
	Open_AppendFile(&FILEname[0]);
	for(currResult = 0;currResult < ChNum;currResult++)
	{
		switch (TestCode[currResult]) {
		case TWO_WIRE_LATCHING:
			MeasuredVal++;
			fMeasured = !(LatchState[currResult] & 255);
			sprintf(Buffer,"Testing Latch\n");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			PortTypes[currResult] = TTLatch;
			break;

		case ONE_VOLT:
		case TWOFIVE_VOLT:
		case THREE_VOLT:
			fMeasured = (float) *MeasuredVal++ / 1000;
			if(*SetVal < 1){
				comp_max = 0.02; //Once this works decrease the multiplication factor to improve testing accuracy
				comp_min = 0.02;
			}else{
				comp_max = 0.01 * (*SetVal);
				comp_min = 0.01 * (*SetVal);
			}
			sprintf(Buffer,"Testing Voltage\n");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			PortTypes[currResult] = TTVoltage;
			break;

		case TWENTY_AMP:
			fMeasured = (float) *MeasuredVal++ / 1000;
			comp_max = 0.03 * (*SetVal); //Once this works decrease the multiplication factor to improve testing accuracy
			comp_min = 0.03 * (*SetVal);
			sprintf(Buffer,"Testing Currrent\n");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			PortTypes[currResult] = TTCurrent;
			break;
		case ASYNC_PULSE:
			fMeasured = (float) *MeasuredVal++ / 1000;
			comp_max = 0;//(*SetVal) + 1; //Once this works decrease the multiplication factor to improve testing accuracy
			comp_min = 0;//(*SetVal) - 1;
			sprintf(Buffer,"Testing Async\n");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			PortTypes[currResult] = TTAsync;
			break;
		case SDI_TWELVE:
			fMeasured = (float) *MeasuredVal++ / 1000;
			comp_max = (*SetVal); //Once this works decrease the multiplication factor to improve testing accuracy
			comp_min = (*SetVal);
			sprintf(Buffer,"Testing SDI-12\n");
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			PortTypes[currResult] = TTSDI;
			break;
		case NOTEST:
			fMeasured = 0;
			MeasuredVal++;
			PortTypes[currResult] = TTNo;
			break;
		}
		if(fMeasured <= (*SetVal + comp_max) && fMeasured >= (*SetVal - comp_min))
		{
			//Pass
			sprintf(Buffer,"**Port %d** PASSED\n",currResult+1);
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			sprintf(Buffer,"Measured Value: %.04f		Set Value: %.04f \n\n",fMeasured , *SetVal); //(float)
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			sprintf(Buffer, ". ");
			LCD_printf(&Buffer[0], strlen(Buffer));
			TresultStatus = TRpassed;
		}
		else
		{
			//Fail
			sprintf(Buffer,"**Port %d** FAILED\n",currResult+1);
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));
			sprintf(Buffer,"Measured Value: %.04f		Set Value: %.04f \n\n",fMeasured , *SetVal); //(float)
			CDC_Transmit_FS(&Buffer[0], strlen(Buffer));

			sprintf(Buffer, "X ");
			LCD_printf(&Buffer[0], strlen(Buffer));
			TestResults[GlobalTestNum] = false;
			TresultStatus = TRfailed;
		}
		if ((currResult < BoardConnected.outputPortCount) && (PortTypes[currResult] == TTLatch)) {
			sprintf(Buffer,
					"%d,%d,L%d,%c,%c,,,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\r\n",
					BoardConnected.BoardType, GlobalTestNum,
					(currResult + 1),
					PortTypes[currResult], TresultStatus, LatchRes.tOn,
					LatchRes.tOff, LatchRes.PortAhighVoltage,
					LatchRes.PortBlowVoltage, LatchRes.PortBhighVoltage,
					LatchRes.PortAlowVoltage, LatchRes.InAvgVoltage,
					LatchRes.InLowVoltage, LatchRes.FuseAvgVoltage,
					LatchRes.FuseLowVoltage, LatchRes.MOSonHigh,
					LatchRes.MOSonLow, LatchRes.MOSoffHigh, LatchRes.MOSoffLow);
		} else if ((PortTypes[currResult] != TTNo)){
			sprintf(Buffer, "%d,%d,%d,%c,%c,%f,%f\r\n", BoardConnected.BoardType, GlobalTestNum, (currResult+1)-BoardConnected.outputPortCount, PortTypes[currResult], TresultStatus,*SetVal,fMeasured);
		}
		Update_File(&FILEname[0], Buffer);
		*SetVal++;
	}
	Close_File(&FILEname[0]);
//	WriteSDresults(&PortTypes[0] ,&SetResults[0], &MeasuredResults[0]);
}
