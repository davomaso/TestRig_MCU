/*
 * interogate_project.h
 *
 *  Created on: Apr 12, 2020
 *      Author: David Mason
 */

//store the various global variables here that will be accessible across the entire project
//once the system is working correctly, pointers should be implemented so that addresses can be passed rather than variables

#ifndef INC_INTEROGATE_PROJECT_H_
#define INC_INTEROGATE_PROJECT_H_

#include "main.h"
#include "Test.h"
#include "Board_Config.h"
#include <time.h>
#include "ADC_Variables.h"

//Routines
void communication_array(uns_ch, uns_ch*, uint8);
void communication_response(TboardConfig *,uns_ch*, uns_ch*, uint8);
void communication_command(uns_ch *);
void SetPara(TboardConfig *,uns_ch);
void BoardConfig(void);
void ADC_Init(void);
void TransmitResults(void);
void PrintLatchResults(void);
void LatchErrorCheck(TboardConfig *);
void printLatchError(uint8 *);
void ADC_MUXsel(uint8);

//Constant Defines
#define LRGBUFFER 254
#define SMLBUFFER 32
#define XSMLBUFFER 16

//variables for communication.c
//variables for communication array/command
uint8 Comlen;
uns_ch Com_buffer[LRGBUFFER];
uint8 Datalen;
uns_ch Data_Buffer[LRGBUFFER];
uint8 Paralen;
uns_ch Para[LRGBUFFER];

//Loom Variables
_Bool CheckLoom;
_Bool LoomChecking;
uint16 LoomCheckCount;
uint8 LoomState;
uint8 PrevLoomState;

//0x1A and 0x1B Command and Response
uint16 sampleTime;
uint16 sampleCount;
_Bool samplesUploading;
_Bool samplesUploaded;
unsigned char sampleBuffer[256];

//Async Variables
typedef struct{
	_Bool Active;
	uint8 PulseCount;
	uint8 fcount;
	uint16 scount;
	_Bool PulseState;
	_Bool FilterEnabled;
}TasyncConfig;

TasyncConfig Async_Port1;
TasyncConfig Async_Port2;
TasyncConfig Async_Port3;
TasyncConfig Async_Port4;
TasyncConfig Async_Port5;
TasyncConfig Async_Port6;
TasyncConfig Async_Port7;
TasyncConfig Async_Port8;
TasyncConfig Async_Port9;

_Bool AsyncComplete;

//SDI Twelve Variables
typedef struct{
	uint8 Address;
	float setValue;
	float measuredValue;
	bool Enabled;
}TsdiConfig;

TsdiConfig SDI_Port1;
TsdiConfig SDI_Port2;
TsdiConfig SDI_Port3;
TsdiConfig SDI_Port4;
TsdiConfig SDI_Port5;
TsdiConfig SDI_Port6;

uint8 SDIAddress;
float SDIMeasurement;
typedef enum {SDSundef, SDSquery, SDSaddress, SDSc, SDSdPending, SDSd}TsdiState;
TsdiState SDSstate;

//Latch Variables
uint16_t LatchCountTimer;
uint8_t stableVoltageCount;
uint16_t latchCountTo;
uint8 LatchTestPort;

#define ADC_BUF_LEN 2048

typedef struct{
	uint32_t total;
	float average;
	uint16_t raw_Buffer[20];
	uint16_t avg_Buffer[ADC_BUF_LEN];
	uint16 currentValue;

	uint8_t HighPulseWidth;
	_Bool HighPulseMeasure;
	uint8_t LowPulseWidth;
	_Bool LowPulseMeasure;

	double highVoltage;
	double lowVoltage;

}TADCconfig;

TADCconfig adc1;
TADCconfig adc2;
TADCconfig Vfuse;
TADCconfig Vmos1;
TADCconfig Vmos2;
TADCconfig Vin;
TADCconfig Vuser;

typedef struct{
	// Latch Pulse Widths and Voltages
	uint8 tOn;
	uint8 tOff;
	float PortAhighVoltage;
	float PortAlowVoltage;
	float PortBhighVoltage;
	float PortBlowVoltage;

	//Vfuse and Vin
	float FuseAvgVoltage;
	float FuseLowVoltage;
	float InAvgVoltage;
	float InLowVoltage;

	//MOSFET Voltages
	float MOSonHigh;
	float MOSonLow;
	float MOSoffHigh;
	float MOSoffLow;
}TlatchResults;

TlatchResults LatchRes;

//uint8 usADCcount; 			//us ADC count to poll the ADC every 100us
uint8_t PulseCountDown;		// Stable voltage pulse

	//Keypad Variables
typedef struct {
	bool Pressed;
	uint8 Count;
}TkeypadConfig;

TkeypadConfig KP_1;
TkeypadConfig KP_2;
TkeypadConfig KP_3;
TkeypadConfig KP_4;
TkeypadConfig KP_5;
TkeypadConfig KP_6;
TkeypadConfig KP_7;
TkeypadConfig KP_8;
TkeypadConfig KP_9;
TkeypadConfig KP_0;
TkeypadConfig KP_hash;
TkeypadConfig KP_star;

int8 CorrectionFactors[36];
uint8 QuitCount;

	//Serial Number variables
uint8 SerialNumber[9];
uint8 SerialCount;
uint8 BarcodeCount;
uint8 BarcodeBuffer[9];
_Bool BarcodeScanned;

//	Timeout functionality, Loop action until timeout is met
bool timeOutEn;
uint32 timeOutCount;

uint8 retryCount;

bool latchTimeOutEn;
bool LatchTimeOut;
uint16 latchTimeOutCount;

bool InputVoltageStable;
uint16 InputVoltageCounter;
uint16 InputVoltageTimer;

uint16 LEDcounter;
bool LED1active;
bool LED2active;
#endif /* INC_INTEROGATE_PROJECT_H_ */
