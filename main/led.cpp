#include <Arduino.h>
#include "preload.h"
#include "led.h"

bool led_state_ = false;
bool initialized_ = false;

void initLed()
{
    if(initialized_)
    {
        pinMode(LED_PIN, OUTPUT);
        setLed(false);
        initialized_ = true;
    }
}

void setLed(bool tf)
{
    led_state_ = tf;
    digitalWrite(LED_PIN, tf ? HIGH : LOW);
}

bool getLed()
{
    return led_state_;
}

