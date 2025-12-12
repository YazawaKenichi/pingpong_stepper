#ifndef __STEPPER_H__
#define __STEPPER_H__

#include "preload.h"

#define DIR_LEFT LOW
#define DIR_RIGHT HIGH

#define STEP_COUNT 200
#define MICROSTEPS 1
#define PI 3.14159265
#define DIRMETER 0.021f
#define LENGTH 1.21f

#define M2STEP(M) ((M) * (STEP_COUNT * MICROSTEPS) / (float) (DIRMETER * PI))
#define STEP2M(STEP) ((STEP) * (DIRMETER * PI) / (float) (STEP_COUNT * MICROSTEPS))

#define SPEED 1 // [m / s]

#define STEP_ON_LOOP 1

void setMicrosteps(unsigned short int);
void initStepper();
float getPos();
void setPos(float);
void oneStep();
void setDir(int);
void executeSteps(unsigned int);
short int checkLimit();
bool getEnable();
void setEnable(bool);
int positionCorrection();
void moveToPosition(float);

#endif

