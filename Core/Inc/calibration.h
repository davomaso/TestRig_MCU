#ifndef INC_CALIBRATION_H_
#define INC_CALIBRATION_H_

#include "interogate_project.h"

#define CalibrateTimerTo 5000

void Calibration();
float Current_calibration(uint8);
float Voltage_calibration(uint8);

typedef struct{
	int8 CalibrationFactor[6];
	bool lowItestComplete;
}TportCalibration;

TportCalibration Port1;
TportCalibration Port2;
TportCalibration Port3;
TportCalibration Port4;
TportCalibration Port5;
TportCalibration Port6;

//Calibration Enums


typedef enum {V_1 = 0, V_05 = 1, V_24 = 2, I_20 = 3, I_4 = 4, I_175 = 5, Done = 6}TcalTestConfig;

bool currentlyCalibrating;
uint16 CalibratingTimer;

uint8 CalibrationCountdown;
bool switchToCurrent;


typedef struct {
	uint32_t total;
	uint16_t average;
}TcalibrateConfig;

TcalibrateConfig calibrateADCval;

#endif /* INC_CALIBRATION_H_ */
