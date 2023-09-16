#include "Ultrasonic.cpp"
#define UMBRAL_TIEMPO_PUERTA_ABIERTA 5000

class UltrasonicDoor : public Ultrasonic
{
private:
    bool doorOpen;
    int timeOpened;

    void startTimer()
    {
        timeOpened = millis();
    }

    bool reachedTimeout()
    {
        if (timeOpened == 0)
        {
            return false;
        }
        int currentTime = millis();
        int timeElapsed = currentTime - timeOpened;
        return timeElapsed > UMBRAL_TIEMPO_PUERTA_ABIERTA;
    }

public:
    UltrasonicDoor(int triggerPin, int echoPin) : Ultrasonic(triggerPin, echoPin)
    {
        doorOpen = false;
        timeOpened = 0;
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
                startTimer();
                return true;
            }
            else if (!isDoorOpen())
            {
                doorOpen = false;
                event = EVENTO_PUERTA_CERRADA;
                return true;
            }
        }
        if (reachedTimeout())
        {
            event = EVENTO_TIMEOUT_CIERRE_PUERTA;
            startTimer();
            return true;
        }
        return false;
    }
};