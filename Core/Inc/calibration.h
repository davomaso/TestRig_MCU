#ifndef INC_CALIBRATION_H_
#define INC_CALIBRATION_H_

#include "interogate_project.h"

#define CALIBRATION_TIMEOUT 10000

void Calibration();
float Current_calibration(uint8);
float Voltage_calibration(uint8);


//Calibration Enums
typedef enum {V_1 = 0, V_05 = 1, V_24 = 2, I_20 = 3, I_4 = 4, I_175 = 5, Done = 6}TcalTestConfig;
uint8 CalibrationStatusRegister;

#define CALIBRATE_VOLTAGE_SET 		(1 << 0)
#define CALIBRATE_CURRENT_SET		(1 << 1)

uint16 CalibratingTimer;
uint8 CalibrationCountdown;

typedef struct {
	uint32_t total;
	uint16_t average;
	uint16 averageBuffer[2000];
}TcalibrateConfig;

TcalibrateConfig calibrateADCval;

#endif /* INC_CALIBRATION_H_ */
