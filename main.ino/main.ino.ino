#include <A4988.h>

const int MOTOR_STEPS = 200;
const int DIR  =  8;
const int STEP =  9;

/* (10/8, 8) (rps, microsteps) <= (10, 1) */
int microsteps = 8;
float rps      = 10 / microsteps;

A4988 stepper(MOTOR_STEPS, DIR, STEP);

void setup() {
  stepper.begin(rps * 60, microsteps);
}

void loop() {
  stepper.rotate(360);
  delay(1000);
  stepper.rotate(-360);
  delay(1000);
}
