#include <main.h>
#include "SetVsMeasured.h"
#include "Global_Variables.h"
#include "Test.h"
#include "File_Handling.h"
#include "LCD.h"
#include "UART_Routine.h"

/*
 * Routine uses the test results and set values to compare the results to determine whether the test passed
 */
void HandleResults(TboardConfig *Board, float *SetVal) {
	uint8 TotalPort = Board->latchPortCount + Board->analogInputCount + Board->digitalInputCout;	// determine how many total ports the boards have
	uint8 ChCount;
	uint8 totalChannel = 0;
	uint8 currentPort;
	uint8 spacing;
	float fMeasured;								// Value measured by target baord
	float range;
	float tolerance;								// Testing tolerance
	tolerance = 0;
	Tresult TresultStatus;

	Open_AppendFile(&SDcard);						// Open the test results file for the board type/serial connected
	sprintf((char*) &debugTransmitBuffer[0], "\n====================	Test %d	====================\n\n",
			Board->GlobalTestNum + 1);
	printT((uns_ch*) &debugTransmitBuffer[0]);
	LCD_ClearLine(2);								// Clear screen, except line 1
	LCD_ClearLine(3);
	LCD_ClearLine(4);
	LCD_setCursor(2, 0);
	sprintf((char*) &debugTransmitBuffer[0], "       Test %d       ", Board->GlobalTestNum + 1);	// Print test being run
	LCD_displayString((uns_ch*) &debugTransmitBuffer[0], strlen((char*) debugTransmitBuffer));
	LCD_ClearLine(3);
	LCD_setCursor(3, 0);

	spacing = (20 - (TotalPort) - (TotalPort - 1));		// Set spacing for the test results of each port on line 3
	spacing = (spacing & 1) ? spacing + 1 : spacing;
	spacing /= 2;
	LCD_setCursor(3, spacing);
	LCD_setCursor(3, spacing);
	for (currentPort = 0; currentPort < TotalPort; currentPort++) {
		TresultStatus = TRpassed;	// Set test to passed by default, disable when failed
		switch (Board->TestCode[currentPort]) {
		case TWO_WIRE_LATCHING:
			printT((uns_ch*) "\nTesting Latch\n");
			PortTypes[currentPort] = TTLatch;
			tolerance = 0;
			ChCount = 2; // Single channel of results, twovolttest.channels is the pulse width of the two wire latch test variable
			break;
		case ONE_VOLT:
		case TWOFIVE_VOLT:
		case THREE_VOLT:
			if (Board->TestCode[currentPort] == ONE_VOLT)	// set range of voltage test depending on the test selected
				range = 1;
			else if (Board->TestCode[currentPort] == TWOFIVE_VOLT )
				range = 2.5;
			else if (Board->TestCode[currentPort] == THREE_VOLT)
				range = 3.0;
			tolerance = GET_VOLTAGE_TEST_TOLERANCE(range, *SetVal);	// get the tolerance for the voltage test
			printT((uns_ch*) "\nTesting Voltage\n");
			PortTypes[currentPort] = TTVoltage;
			ChCount = OnevoltTest.Channels;							// Set the amount of chs required for sampling
			break;

		case TWENTY_AMP:
			tolerance = GET_CURRENT_TEST_TOLERANCE(*SetVal); // get the tolerance for the current test
			printT((uns_ch*) "\nTesting Currrent\n");
			PortTypes[currentPort] = TTCurrent;
			ChCount = currentTest.Channels;					// Set the amount of chs required for sampling
			break;
		case ASYNC_PULSE:
			tolerance = 0; 									// No tolerance as test is digital
			printT((uns_ch*) "\nTesting Async\n");
			PortTypes[currentPort] = TTAsync;
			ChCount = asyncFilteredTest.Channels;
			break;
		case SDI_TWELVE:
			tolerance = 0; 									// No tolerance as test is digital
			printT((uns_ch*) "\nTesting SDI-12\n");
			PortTypes[currentPort] = TTSDI;
			ChCount = sdi12Test.Channels;
			break;
		case AQUASPY:
			tolerance = 0; 									// No tolerance as test is digital
			printT((uns_ch*) "\nTesting RS485\n");
			PortTypes[currentPort] = TTRS485;
			ChCount = rs485Test.Channels;
			break;
		case NOTEST:
			fMeasured = (float) 0;
			PortTypes[currentPort] = TTNoTest;
			SetVal += 2;
			totalChannel += 2; 									// increment total ch by two when notest is enabled
			break;
		}
		if (PortTypes[currentPort] != TTNoTest) {
			sprintf((char*) &debugTransmitBuffer[0], "****   Port %d   ****\nSet Value:		Measured Value:\n",
					currentPort + 1);
			printT((uns_ch*) &debugTransmitBuffer);
			// Sort through the results
			for (uint8 currCh = 0; currCh < ChCount; currCh++) {
				fMeasured = (float) Board->TestResults[Board->GlobalTestNum][totalChannel++] / 1000;	// results returned in base 1000, divide by 1000 to get in floating form
				if (CompareSetMeasured(SetVal, fMeasured, tolerance) == 'f') {	// if f is returned from comparison test failed, print results to terminal and store failed test
					TresultStatus = TRfailed;
					sprintf((char*) &debugTransmitBuffer[0], "     %.03f                 %.03f 				X\n", *SetVal,
							fMeasured); //(float)
				} else
					sprintf((char*) &debugTransmitBuffer[0], "     %.03f                 %.03f \n", *SetVal, fMeasured); //(float)
				printT((uns_ch*) &debugTransmitBuffer);	// print results to terminal

				SetVal++;	//increment the setvalue to next
				if (PortTypes[currentPort] == TTLatch) {
					totalChannel++;
					SetVal++;
					break;
				}
			}
		}
		// Print Pass/Fail to the LCD screen
		if (TresultStatus == TRpassed) {
			sprintf((char*) &lcdBuffer[0], "****   PASSED   ****\n\n");
			sprintf((char*) &lcdBuffer[0], ". ");
			LCD_displayString((uns_ch*) &lcdBuffer[0], strlen((char*) &lcdBuffer));
		} else if (TresultStatus == TRfailed) {
			sprintf((char*) &lcdBuffer[0], "****   FAILED   ****\n\n");
			sprintf((char*) &lcdBuffer[0], "X ");
			LCD_displayString((uns_ch*) &lcdBuffer[0], strlen((char*) &lcdBuffer));
			SET_BIT(Board->TPR, (1 << Board->GlobalTestNum));
		}
		//Write Results to file
		if ((currentPort < Board->latchPortCount) && (PortTypes[currentPort] == TTLatch)) {	// Test run is latch test so print the various parameters required for data analysis
			sprintf((char*) &TestResultsBuffer[0],	// store all the latch reults, in a buffer
					"%x,%d,L%d,%c,%c,,,%d,%.3f,%.3f,%.3f,%.3f,%d,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\r\n",
					Board->BoardType, Board->GlobalTestNum + 1, (currentPort + 1), PortTypes[currentPort],
					TresultStatus, LatchRes.tOn, LatchPortA.highVoltage, LatchPortB.lowVoltage,
					MOSFETvoltageA.highVoltage, MOSFETvoltageB.lowVoltage, LatchRes.tOff, LatchPortB.highVoltage,
					LatchPortA.lowVoltage, MOSFETvoltageB.highVoltage, MOSFETvoltageA.lowVoltage, Vin.average,
					Vin.lowVoltage, Vfuse.average, Vfuse.lowVoltage);
		} else if ((PortTypes[currentPort] != TTNoTest) && (PortTypes[currentPort] != TTRS485)) {
			sprintf((char*) &TestResultsBuffer[0], "%x,%d,%d,%c,%c,%f,%f\r\n", Board->BoardType,
					Board->GlobalTestNum + 1, (currentPort + 1) - Board->latchPortCount, PortTypes[currentPort],
					TresultStatus, CHval[Board->GlobalTestNum][currentPort], fMeasured);
		} else if ((PortTypes[currentPort] != TTNoTest)) {
			sprintf((char*) &TestResultsBuffer[0], "%x,%d,%d,%c,%c\r\n", Board->BoardType, Board->GlobalTestNum + 1,
					(currentPort + 1) - Board->latchPortCount, PortTypes[currentPort], TresultStatus);
		}
		if ((PortTypes[currentPort] != TTNoTest))
			Update_File(&SDcard, (char*) &SDcard.FILEname[0], (char*) &TestResultsBuffer[0]);	// write results to file if not NoTest
	}
	Close_File(&SDcard);	//Close file following storage of results
	// Store battery voltage each test
	Board->rawBatteryLevel[Board->GlobalTestNum] = Board->TestResults[Board->GlobalTestNum][16];	// battery voltage always in the 17th position of sampling string
}

// Routine to compare the results of the test, set v measured using the tolerance passed.
Tresult CompareSetMeasured(float *Set, float Measured, float tolerance) {
	if (((*Set - tolerance) <= Measured) && ((*Set + tolerance) >= Measured))
		return TRpassed; // return passed if result falls within tolerances
	else
		return TRfailed;
}
