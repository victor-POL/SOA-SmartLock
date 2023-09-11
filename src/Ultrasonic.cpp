#include <Arduino.h>
#include "States.h"
#include "Events.h"

#define UMBRAL_PERSONA_DETECTADA 100

extern int event;

class Ultrasonic
{
private:
    int triggerPinSelected;
    int echoPinSelected;

    float previousDistance;

    bool personDetected;

public:
    Ultrasonic(int echoPin, int triggerPrin)
    {
        triggerPinSelected = echoPin;
        echoPinSelected = triggerPrin;
        personDetected = false;
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

    bool checkStatus()
    {
        float actualDistance = getDistance();
        float previousDistance = this->previousDistance;

        if (actualDistance != previousDistance)
        {
            this->previousDistance = actualDistance;

            if (personDetected)
            {
                if (actualDistance > UMBRAL_PERSONA_DETECTADA)
                {
                    personDetected = false;
                    event = EVENTO_PERSONA_SE_FUE;
                }
                else
                {
                    if (actualDistance < UMBRAL_PERSONA_DETECTADA)
                    {
                        event = EVENTO_CONTINUE;
                    }
                }
            }

            else
            {
                if (actualDistance < UMBRAL_PERSONA_DETECTADA)
                {
                    personDetected = true;
                    event = EVENTO_PERSONA_DETECTADA;
                }
                else
                {
                    event = EVENTO_CONTINUE;
                }
            }
            return true;
        }

        return false;
    }
};