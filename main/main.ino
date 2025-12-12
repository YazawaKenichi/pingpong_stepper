#include <math.h>
#include <stdio.h>

#define DIR_PIN 8
#define STEP_PIN 9
#define ENABLE_PIN 7
#define SLEEP_PIN 13
#define MS1_PIN 10
#define MS2_PIN 11
#define MS3_PIN 12

#define LEFT_LIMIT_PIN 2
#define RIGHT_LIMIT_PIN 3

#define DIR_LEFT LOW
#define DIR_RIGHT HIGH

#define STEP_COUNT 200
#define MICROSTEPS 1
#define PI 3.14159265
#define DIRMETER 0.021f
#define LENGTH 2.0f

#define M2STEP(M) ((M) * (STEP_COUNT * MICROSTEPS) / (float) (DIRMETER * PI))
#define STEP2M(STEP) ((STEP) * (DIRMETER * PI) / (float) (STEP_COUNT * MICROSTEPS))

#define SPEED 1 // [m / s]

void initStepper();
float getPos();
void setPos(float);
void oneStep();
void setDir(int);
void executeSteps(unsigned int);
short int checkLimit();
void moveToPosition(float);

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

    char buf_target[16];
    char buf_pos[16];
    char buf_diff[16];
    dtostrf(target, 0, 4, buf_target);
    dtostrf(pos_, 0, 4, buf_pos);
    dtostrf(diff, 0, 4, buf_diff);
    printf("target: %f, pos_: %f, diff: %f, dir: %d, step: %ld, unit: %d\r\n", buf_target, buf_pos, buf_diff, dir, step, unit);
}

float target;   //! [ m ]
String rxLine;

FILE serial_stdout;
int serial_putchar(char c, FILE* f)
{
    Serial.write(c);
    return 0;
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting Programs");

    fdev_setup_stream(&serial_stdout, serial_putchar, NULL, _FDEV_SETUP_WRITE);
    stdout = &serial_stdout;

    Serial1.begin(115200);
    initStepper();
    target = 0;

    Serial.println("Setup Stop");
}

void loop()
{
    while (Serial1.available())
    {
        char c = Serial1.read();

        if (c == '\r') continue;

        if (c == '\n')
        {
            if (rxLine.length() > 0)
            {
                target = rxLine.toFloat();
                Serial.print("target(m) = ");
                Serial.println(target, 4);
                rxLine = "";
            }
        }
        else
        {
            rxLine += c;
        }
    }
    moveToPosition(target);
}

