/*
 * StateMachine.h
 *
 *  Created on: 3 Sep 2021
 *      Author: mason
 */

#ifndef INC_STATEMACHINE_H_
#define INC_STATEMACHINE_H_

void handleIdle(TboardConfig *, TprocessState *);
void handleSolarCharger(TboardConfig *, TprocessState *);
void handleInputVoltage(TboardConfig *, TprocessState *);
void handleInitialising(TboardConfig *, TprocessState *);
void handleTestBegin(TboardConfig *, TprocessState *);
void handleProgramming(TboardConfig *, TprocessState *);
void handleCalibrating(TboardConfig *, TprocessState *);
void handleInterogating(TboardConfig *, TprocessState *);
void handleConfiguring(TboardConfig *, TprocessState *);
void handleLatchTest(TboardConfig *, TprocessState *);
void handleAsyncTest(TboardConfig *, TprocessState *);
void handleSampling(TboardConfig *, TprocessState *);
void handleUploading(TboardConfig *, TprocessState *);
void handleSortResults(TboardConfig *, TprocessState *);
void handleSerialise(TboardConfig *, TprocessState *);

#endif /* INC_STATEMACHINE_H_ */
