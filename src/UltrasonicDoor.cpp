#include "Ultrasonic.cpp"

class UltrasonicDoor : public Ultrasonic
{
private:
    bool doorOpen;

public:
    UltrasonicDoor(int triggerPin, int echoPin) : Ultrasonic(triggerPin, echoPin)
    {
        doorOpen = false;
    }

    bool isDoorOpen()
    {
        float currentDistance = getDistance();
        this->previousDistance = currentDistance;
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
            if (isDoorOpen() && !doorOpen)
            {
                doorOpen = true;
                event = EVENTO_PUERTA_ABIERTA;
                return true;
            }
            else if (!isDoorOpen() && doorOpen)
            {
                doorOpen = false;
                event = EVENTO_PUERTA_CERRADA;
                return true;
            }
        }
        return false;
    }
};