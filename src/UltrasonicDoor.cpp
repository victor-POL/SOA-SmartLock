#include "Ultrasonic.cpp"

class UltrasonicDoor : public Ultrasonic
{
public:
    UltrasonicDoor(int triggerPin, int echoPin) : Ultrasonic(triggerPin, echoPin)
    {
    };

    bool isDoorOpen()
    {
        float currentDistance = getDistance();
        float previousDistance = this->previousDistance;
        if (currentDistance > UMBRAL_PUERTA_ABIERTA)
        {
            return true;
        }
        return false;
    }

    bool checkStatus()
    {
        float currentDistance = getDistance();
        float previousDistance = this->previousDistance;
        if (currentDistance != previousDistance)
        {
            event = isDoorOpen()? EVENTO_PUERTA_ABIERTA : event;
            return true;
        }
        return false;
    }
};