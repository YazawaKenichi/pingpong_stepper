#include <Arduino.h>

float target_;   //! [ m ]
String rxLine_;

void initSerial()
{
    Serial.begin(115200);
    Serial.println("Starting Programs");
    Serial1.begin(115200);
    Serial.println("Setup Stop");
}

void initTarget()
{
    target_ = 0;
}

void setTarget(float t_)
{
    target_ = t_;
}

float getTarget()
{
    return target_;
}

void updateTarget()
{
    while(Serial1.available())
    {
        char c = Serial1.read();

        if(c == '\r') continue;

        if(c == '\n')
        {
            if(rxLine_.length() > 0)
            {
                target_ = rxLine_.toFloat();
                Serial.print("target(m) = ");
                Serial.println(target_, 4);
                rxLine_ = "";
            }
        }
        else
        {
            rxLine_ += c;
        }
    }
}

