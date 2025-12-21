#include <Arduino.h>
#include <math.h>
#include "serial.h"
#include "stepper.h"
#include "led.h"

float pos_;
bool enable_;
bool goal_;
// const float dt_ = (1e6f / (float) M2STEP(SPEED)) / 2;
const float dt_ = 2000;

void setMicrosteps(unsigned short int ms_)
{
    switch(ms_)
    {
        case 2:
            digitalWrite(MS1_PIN, HIGH);
            digitalWrite(MS2_PIN, LOW);
            digitalWrite(MS3_PIN, LOW);
            break;
        case 4:
            digitalWrite(MS1_PIN, LOW);
            digitalWrite(MS2_PIN, HIGH);
            digitalWrite(MS3_PIN, LOW);
            break;
        case 8:
            digitalWrite(MS1_PIN, HIGH);
            digitalWrite(MS2_PIN, HIGH);
            digitalWrite(MS3_PIN, LOW);
            break;
        case 16:
            digitalWrite(MS1_PIN, HIGH);
            digitalWrite(MS2_PIN, HIGH);
            digitalWrite(MS3_PIN, HIGH);
            break;
        default:
            digitalWrite(MS1_PIN, LOW);
            digitalWrite(MS2_PIN, LOW);
            digitalWrite(MS3_PIN, LOW);
            break;
    }
}

void initStepper()
{
    pinMode(TRIGGER_PIN, OUTPUT);
    digitalWrite(TRIGGER_PIN, HIGH);

    pinMode(DIR_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);

    pinMode(LEFT_LIMIT_PIN, INPUT_PULLUP);
    pinMode(RIGHT_LIMIT_PIN, INPUT_PULLUP);

    digitalWrite(DIR_PIN, DIR_RIGHT);
    digitalWrite(STEP_PIN, LOW);

    pinMode(ENABLE_PIN, OUTPUT);
    pinMode(SLEEP_PIN, OUTPUT);
    pinMode(MS1_PIN, OUTPUT);
    pinMode(MS2_PIN, OUTPUT);
    pinMode(MS3_PIN, OUTPUT);

    setMicrosteps(MICROSTEPS);

    digitalWrite(SLEEP_PIN, HIGH);

    setEnable(false);
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

void writeDir(int dir)
{
    digitalWrite(DIR_PIN, dir);
}

void executeSteps(unsigned int steps)
{
    writeDir(getDir());
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

void positionCorrection()
{
    // リミットスイッチに触れていたら方向に応じて補正
    short int limit_ = checkLimit();
    if(limit_ < 0)
    {
        setPos(0);
    }
    if(0 < limit_)
    {
        setPos(LENGTH);
    }
}

bool getEnable()
{
    return enable_;
}

void setEnable(bool tf)
{
    enable_ = tf;
    digitalWrite(ENABLE_PIN, tf ? LOW : HIGH);
    delayMicroseconds(1);
}

void goalFeedback()
{
    digitalWrite(TRIGGER_PIN, getGoal() ? HIGH : LOW);
}

bool getGoal()
{
    return goal_;
}

void setGoal(bool tf)
{
    goal_ = tf;
    goalFeedback();
    setLed(tf);
}

float diff_;
int dir_;
unsigned long int step_;

void updateDir(float d_)
{
    dir_ = (d_ >= 0) ? DIR_RIGHT : DIR_LEFT;
}

int getDir()
{
    return dir_;
}

void updateStep(float d_)
{
#if STEP_ON_LOOP
    step_ = 1;
#else
    step_ = lround(fabs(M2STEP(d_)));
#endif
}

unsigned long int getStep()
{
    return step_;
}

void updateDiff()
{
    diff_ = getTarget() - getPos();
}

float getDiff()
{
    return diff_;
}

void updateDDS()
{
    updateDiff();
    updateDir(getDiff());
    updateStep(getDiff());
}

void moveToPosition()
{
    if (target < 0.0f) target = 0.0f;
    if (target > LENGTH) target = LENGTH;

    Serial.println("updateDDS();");
    updateDDS();

    delay(500);

    unsigned long int s = getStep();
    int unit = (getDiff() >= 0) ? 1 : -1;
    setPos(getPos() + (unit * STEP2M(s)));

    executeSteps(s);

    setEnable(false);
    setGoal(true);
}

