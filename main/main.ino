#include "serial.h"
#include "stepper.h"
#include "led.h"

float target;   //! [ m ]
String rxLine;

void setup()
{
    initSerial();
    initLed();
    initTarget();

    initStepper();
}

void loop()
{
    updateTarget();
    moveToPosition(getTarget());
    // positionCorrection();
    goalFeedback();
}

