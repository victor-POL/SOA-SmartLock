#include <Arduino.h>
#include "States.h"
#include "Events.h"

#define UMBRAL_PERSONA_DETECTADA 50
#define UMBRAL_PUERTA_ABIERTA 5

extern int event;

class Ultrasonic
{
protected:
    int triggerPinSelected;
    int echoPinSelected;

    float previousDistance;

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

        long duration_us = pulseIn(echoPinSelected, HIGH);

        float distance_cm = 0.017 * duration_us;

        return distance_cm;
    }

    float getPreviousDistance()
    {
        return previousDistance;
    }

    void setPreviousDistance(float distance)
    {
        previousDistance = distance;
    }
};