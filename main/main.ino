#include <A4988.h>

#define RECEIVE_FLOAT 1
#define RECEIVE_STRING 0

const int MOTOR_STEPS = 200;
const int DIR  =  8;
const int STEP =  9;

/* (10/8, 8) (rps, microsteps) <= (10, 1) */
int microsteps = 8;
float rps      = 10 / microsteps;

String line = "";
float stepper_position_ = 0.0f;

A4988 stepper(MOTOR_STEPS, DIR, STEP);

void setup()
{
    Serial.begin(115200);
    Serial1.begin(115200);
    stepper.begin(rps * 60, microsteps);
}

void stepper_move(float value)
{
    Serial.print("Value = ");
    Serial.println(value, 6);
}

void loop()
{
    float value;
#if RECEIVE_FLOAT
#if 0
    //! float の型をそのまま TX しようとしたパターン
    //! 問題点 : Serial1.available() >= 4 すると テキトーに溜まった 4 バイトを出力することになるらしい
    //!             それが原因で多分 microROS 側のシリアルを変に取得して変な値を出力してしまう
    if(Serial1.available() >= 4)
    {
        uint8_t buffer[4];
        Serial1.readBytes(buffer, 4);

        memcpy(&value, buffer, 4);

        Serial.print("Received float: ");
        Serial.println(value, 6);
        Serial.println();

        stepper.rotate(value);
    }
#endif
    if(Serial1.available() > 0)
    {
        char c = Serial1.read();
        Serial.println("Available");
        if(c == '\n')
        {
            Serial.println("LF");
            if(line.length() > 0)
            {
                Serial.println("line.length() > 0");
                stepper_position_ = line.toFloat();
                stepper_move(stepper_position_);
            }
            line = "";
        }
        else if(c != '\r')
        {
            Serial.println("line += c");
            line += c;
        }
    }
#endif
#if RECEIVE_STRING
    if(Serial1.available())
    {
        char c = Serial1.read();
        Serial.write(c);
    }
#endif
}

