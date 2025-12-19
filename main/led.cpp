#include <Arduino.h>
#include "led.h"

bool led_state_ = false;
bool initialized_ = false;

void initLed()
{
    if(initialized_)
    {
        pinMode(LED_BUILTIN, OUTPUT);
        setLed(false);
        initialized_ = true;
    }
}

void setLed(bool tf)
{
    led_state_ = tf;
    digitalWrite(LED_BUILTIN, tf ? HIGH : LOW);
}

bool getLed()
{
    return led_state_;
}

