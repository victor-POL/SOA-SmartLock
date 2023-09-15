#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>
#include "States.h"
#include "Events.h"

#define UMBRAL_PERSONA_DETECTADA 50
#define UMBRAL_PUERTA_ABIERTA 20

extern int event;

class Ultrasonic
{
protected:
    int triggerPinSelected;
    int echoPinSelected;

    int previousDistance;

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

    int getDistance()
    {
        digitalWrite(triggerPinSelected, LOW);
        delayMicroseconds(2);

        digitalWrite(triggerPinSelected, HIGH);
        delayMicroseconds(10);

        digitalWrite(triggerPinSelected, LOW);

        return 0.01723 * pulseIn(echoPinSelected, HIGH);
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

#endif