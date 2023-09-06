#include <Arduino.h>

class Ultrasonic
{
private:
    int triggerPinSelected;
    int echoPinSelected;
    float duration_us;
    float distance_cm;

public:
    Ultrasonic(int echoPin, int triggerPrin)
    {
        triggerPinSelected = echoPin;
        echoPinSelected = triggerPrin;
    }

    void setup()
    {
        pinMode(triggerPinSelected, OUTPUT);
        pinMode(echoPinSelected, INPUT);
    }

    float getDistance()
    {
        digitalWrite(triggerPinSelected, HIGH);
        delayMicroseconds(10);
        digitalWrite(triggerPinSelected, LOW);

        duration_us = pulseIn(echoPinSelected, HIGH);

        distance_cm = 0.017 * duration_us;

        return distance_cm;
    }
};