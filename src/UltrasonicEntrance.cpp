#include "Ultrasonic.cpp"

class UltrasonicEntrance : public Ultrasonic
{
public:
    UltrasonicEntrance(int triggerPin, int echoPin) : Ultrasonic(triggerPin, echoPin) 
    {
        
    }

    bool checkStatus()
    {
        float currentDistance = getDistance();
        float previousDistance = this->previousDistance;

        if (currentDistance != previousDistance)
        {
            this->previousDistance = currentDistance;
            event = currentDistance < UMBRAL_PERSONA_DETECTADA?
                EVENTO_PERSONA_DETECTADA:
                EVENTO_PERSONA_NO_DETECTADA;
            return true;
        }

        return false;
    }
};