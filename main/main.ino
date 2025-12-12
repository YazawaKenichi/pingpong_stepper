#include "stepper.h"

float target;   //! [ m ]
String rxLine;

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting Programs");

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
    // positionCorrection();
}

