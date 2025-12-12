#include <Arduino.h>
#include <math.h>
#include "stepper.h"

float pos_;
const float dt_ = (1e6f / (float) M2STEP(SPEED)) / 2;

void initStepper()
{
    pinMode(DIR_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(ENABLE_PIN, OUTPUT);
    digitalWrite(ENABLE_PIN, LOW); // A4988 有効 (基板によっては HIGH が有効のこともある)

    pinMode(LEFT_LIMIT_PIN, INPUT_PULLUP);
    pinMode(RIGHT_LIMIT_PIN, INPUT_PULLUP);

    digitalWrite(DIR_PIN, DIR_RIGHT);
    digitalWrite(STEP_PIN, LOW);

    pinMode(ENABLE_PIN, OUTPUT);
    pinMode(SLEEP_PIN, OUTPUT);
    pinMode(MS1_PIN, OUTPUT);
    pinMode(MS2_PIN, OUTPUT);
    pinMode(MS3_PIN, OUTPUT);

    digitalWrite(ENABLE_PIN, LOW);
    digitalWrite(SLEEP_PIN, HIGH);
    digitalWrite(MS1_PIN, LOW);
    digitalWrite(MS2_PIN, LOW);
    digitalWrite(MS3_PIN, LOW);

    pos_ = 0;
}

float getPos()
{
    return pos_;
}

void setPos(float p)
{
    pos_ = p;
}

void oneStep()
{
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(dt_);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(dt_);
}

void setDir(int dir)
{
    digitalWrite(DIR_PIN, dir);
}

void executeSteps(unsigned int steps)
{
    for (unsigned int x = 0; x < steps; x++)
    {
        oneStep();
    }
}

short int checkLimit()
{
    ///// 未実装 /////

    //! リミットスイッチに触れているかチェックする
    //! 左側で -1 右側で +1 触れていなくて 0
    return 0;
}

void moveToPosition(float target)
{
    if (target < 0.0f) target = 0.0f;
    if (target > LENGTH) target = LENGTH;

    float diff = target - getPos();
    int dir = (diff >= 0) ? DIR_RIGHT : DIR_LEFT;
    unsigned long int step = lround(fabs(M2STEP(diff)));
    setDir(dir);
    executeSteps(step);

    int unit = (diff >= 0) ? 1 : -1;
    setPos(getPos() + (unit * STEP2M(step)));
}

