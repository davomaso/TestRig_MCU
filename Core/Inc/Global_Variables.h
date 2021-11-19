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
#include "Global_Variables.h"
#include "Test.h"
#include "Board_Config.h"
#include <time.h>
#include "ADC_Variables.h"

//Routines
void communication_array(uns_ch, uns_ch*, uint8);
void communication_response(TboardConfig*, uns_ch*, uns_ch*, uint8);
void communication_command(uns_ch*);
void SetPara(TboardConfig*, uns_ch);
void BoardConfig(void);
void ADC_Init(void);
void PrintLatchResults(void);
void LatchErrorCheck(TboardConfig*);
//void printLatchError(uint8 *);
void ADC_MUXsel(uint8);

//Constant Defines
#define LRGBUFFER 254
#define SMLBUFFER 32
#define XSMLBUFFER 16

//variables for communication.c
//variables for communication array/command
uint8 Datalen;
uns_ch Data_Buffer[LRGBUFFER];
uint8 BoardCommsParametersLength;
uns_ch BoardCommsParameters[LRGBUFFER];

//Loom Variables
uint16 CheckLoomTimer;
uint8 LoomState;

//0x1A and 0x1B Command and Response
uint16 sampleTime;
uint16 sampleCount;
_Bool samplesUploading;
_Bool samplesUploaded;
unsigned char sampleBuffer[256];

uint8 AsyncDebounceBuffer[5][16];
//Async Variables
typedef struct {
	_Bool Active;
	uint8 PulseCount;
	uint8 fcount;
	uint16 scount;
	_Bool PulseState;
	_Bool FilterEnabled;
	uint8 UnfilteredClkDiv;
	uint8 FilteredCount;

	uint16_t Pin;
	GPIO_TypeDef *Port;
} TasyncConfig;
_Bool AsyncComplete;

//SDI Twelve Variables
typedef struct {
	uint8 Address;
	float setValue;
	float measuredValue;
	bool Enabled;
} TsdiConfig;

uint8 SDIAddress;
float SDIMeasurement;
typedef enum {
	SDSundef, SDSquery, SDSaddress, SDSc, SDSdPending, SDSd
} TsdiState;
TsdiState SDSstate;

typedef struct {
	TasyncConfig Async;
	TsdiConfig Sdi;
	int8 CalibrationFactor[6];
	_Bool lowItestComplete;
} TaportConfig;

TaportConfig Port[9];

typedef enum {
	RSundef, RSquery, RS9pending, RS9, RSMpending, RSM, RSdPending, RSd
} TrsState;
TrsState RSstate;
uns_ch RS485buffer[128];
float RS485sensorBuffer[9];


//Latch Variables
uint16_t LatchCountTimer;
uint8_t stableVoltageCount;
uint16_t latchCountTo;
uint8 LatchTestPort;
bool latchTimeOutEn;
bool LatchTimeOut;
uint16 latchTimeOutCount;


#define ADC_BUF_LEN 2048

typedef struct {
	uint32_t total;
	float average;
	uint32_t avg_Buffer[ADC_BUF_LEN];
	uint16 readValue;
	uint32_t currentValue;

	uint8_t HighPulseWidth;
	_Bool HighPulseMeasure;
	uint8_t LowPulseWidth;
	_Bool LowPulseMeasure;
	float highVoltage;
	float lowVoltage;

} TADCconfig;

TADCconfig LatchPortA;
TADCconfig LatchPortB;
TADCconfig Vfuse;
TADCconfig MOSFETvoltageA;
TADCconfig MOSFETvoltageB;
TADCconfig Vin;
TADCconfig Vuser;

typedef struct {
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
} TlatchResults;

TlatchResults LatchRes;

//uint8 usADCcount; 			//us ADC count to poll the ADC every 100us
uint8_t PulseCountDown;		// Stable voltage pulse

//Keypad Variables
typedef struct {
	uint16_t RowPin;
	GPIO_TypeDef *RowPort;
	uint16_t ColPin;
	GPIO_TypeDef *ColPort;
	bool PreviousState;
	bool State;
	bool Pressed;
	uint8 debounceCount;
} TkeypadConfig;

TkeypadConfig KP[12];
typedef enum { star = 10, hash = 11 }TkeypadSpecialChars;

//Voltage Test Enum
typedef enum {V_12output = 0, V_12 = 1, V_3 = 2, V_105 = 3, V_trim = 4, V_INPUT = 5, V_SOLAR = 6}TvoltageTestVariables;
float setSampleVoltages[7];

typedef enum {BatchMode = 0 , OldBoardMode = 1, VerboseMode = 2, SerialiseMode = 3}TtestMode;

TtestMode TestRigMode;

//Serial Number variables
uint8 SerialNumber[9];
uint8 SerialCount;
uint8 BarcodeCount;
uint8 BarcodeBuffer[9];
_Bool BarcodeScanned;

//	Timeout functionality, Loop action until timeout is met
bool timeOutEn;
uint32 timeOutCount;

bool terminalTimeOutEn;
uint32 terminalTimeOutCount;

uint16 ProgrammingTimeOut;

uint8 retryCount;
uint16 BoardResetTimer;

_Bool InputVoltageStable;
uint16 InputVoltageCounter;
uint16 InputVoltageTimer;
_Bool InputVoltageSampling;

_Bool SolarChargerStable;
uint16 SolarChargerCounter;
uint16 SolarChargerTimer;
_Bool SolarChargerSampling;

extern _Bool OutputsSet;

_Bool RelayPort_Enabled;
uint8 RelayCount;

uint16 LEDcounter;
bool LED1active;
bool LED2active;
#endif /* INC_INTEROGATE_PROJECT_H_ */
