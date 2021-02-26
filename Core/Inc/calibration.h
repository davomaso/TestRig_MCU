#ifndef INC_CALIBRATION_H_
#define INC_CALIBRATION_H_

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
typedef enum {Port_1 = 0, Port_2 = 1, Port_3 = 2, Port_4 = 3, Port_5 = 4, Port_6 = 5}TcalPortConfig;
TcalPortConfig calPort;

typedef enum {V_1 = 0, V_05 = 1, V_24 = 2, I_20 = 3, I_4 = 4, I_175 = 5, Done = 6}TcalTestConfig;
TcalTestConfig calTest;

uint32 DACval;
int calCount;

#endif /* INC_CALIBRATION_H_ */
