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

//Routines
void communication_array(uns_ch, uns_ch*, uint8);
void communication_response(uns_ch*, uns_ch*, uint8);
void communication_command(uns_ch *);
void SetPara(uns_ch);
void BoardConfig(void);
void ADC_Init(void);
void TransmitResults(void);
void PrintLatchResults(void);
int8_t LatchErrorCheck(void);
void printLatchError(uint8 *);
void ADC_MUXsel(uint8);
uint32 ReadSerialNumber(uns_ch *, uint16);

//Constant Defines
#define LRGBUFFER 254
#define SMLBUFFER 32
#define XSMLBUFFER 16


int intBuffer;

//variables for communication.c
//variables for communication array/command
uns_ch Comlen;
uns_ch Com_buffer[LRGBUFFER];
uint8 Paralen;
uns_ch Para[SMLBUFFER];

//communication response
uint16 Board;
uint8 Version;
uns_ch Flags;
uns_ch Subclass;

//Loom Variables
_Bool CheckLoom;
_Bool LoomChecking;
uint16 LoomCheckCount;
uint8 LoomState;

uint8 PrevLoomState;

//Test Parameters Returned
uint16 Samplerate;

//0x1A and 0x1B Command and Response
uint16 sampleTime;
uint16 sampleCount;
_Bool samplesUploading;
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
_Bool LatchPort1;
_Bool LatchPort2;
_Bool LatchPort3;
_Bool LatchPort4;
uint16_t LatchCountTimer;
_Bool LatchSampling;
uint8_t stableVoltageCount;
uint16_t latchCountTo;

#define ADC_BUF_LEN 2048

typedef struct{
	uint32_t total;
	uint16_t average;
	uint16_t raw_Buffer[20];
	uint16_t avg_Buffer[ADC_BUF_LEN];

	uint8_t HighPulseWidth;
	_Bool HighPulseMeasure;
	uint8_t LowPulseWidth;
	_Bool LowPulseMeasure;

	float highVoltage;
	float lowVoltage;
	float steadyState;

}TADCconfig;

TADCconfig adc1;
TADCconfig adc2;
TADCconfig Vfuse;
TADCconfig Vmos1;
TADCconfig Vmos2;
TADCconfig Vin;

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

uint8_t raw_adcCount;
_Bool LatchOnComplete;
_Bool LatchOffComplete;
uint8_t PulseCountDown;

uint8_t LatchErrCheck;

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

bool latchTimeOutEn;
bool LatchTimeOut;
uint16 latchTimeOutCount;


#endif /* INC_INTEROGATE_PROJECT_H_ */
