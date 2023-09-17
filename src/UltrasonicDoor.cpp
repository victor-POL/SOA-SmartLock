#include "Ultrasonic.cpp"
#define UMBRAL_TIEMPO_PUERTA_ABIERTA 10000
#define UMBRAL_TIEMPO_PUERTA_CERRADA 5000
#define DOOR_OPEN 1
#define DOOR_CLOSED 0

class UltrasonicDoor : public Ultrasonic
{
private:
    int timeOpened;
    int timeClosed;
    bool timerClosedActivated;
    bool status;

    void startTimerOpened()
    {
        timeOpened = millis();
    }

    void startTimerClosed()
    {
        timeClosed = millis();
    }

    bool reachedTimeoutClosed()
    {
        int currentTime = millis();
        int timeElapsed = currentTime - timeClosed;
        return timeElapsed > UMBRAL_TIEMPO_PUERTA_CERRADA;
    }

public:
    UltrasonicDoor(int triggerPin, int echoPin) : Ultrasonic(triggerPin, echoPin)
    {
        previousDistance = 0;
        timeOpened = 0;
        timeClosed = 0;
        status = DOOR_CLOSED;
        timerClosedActivated = false;
    }

    bool checkStatus()
    {
        int currentDistance = getDistance();
        int previousDistance = this->previousDistance;

        if (timerClosedActivated == true && reachedTimeoutClosed())
        {
            timerClosedActivated = false;
            status = DOOR_CLOSED;
            event = EVENTO_SE_CERRO_PUERTA;
            return true;
        }

        if (currentDistance != previousDistance)
        {
            this->previousDistance = currentDistance;
            bool isDoorDetected = currentDistance < UMBRAL_PUERTA_ABIERTA;

            if (isDoorDetected == false && status == DOOR_CLOSED)
            {
                status = DOOR_OPEN;
                event = EVENTO_SE_ABRIO_PUERTA;
                return true;
            }
            else if (isDoorDetected == false && status == DOOR_OPEN && timerClosedActivated == true)
            {
                timerClosedActivated = false;
            }
            else if (isDoorDetected == true && status == DOOR_OPEN && timerClosedActivated == false)
            {
                startTimerClosed();
                timerClosedActivated = true;
            }
        }

        return false;
    }
};