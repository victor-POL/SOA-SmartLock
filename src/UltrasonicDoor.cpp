#include "Ultrasonic.cpp"
#define UMBRAL_TIEMPO_PUERTA_ABIERTA 10000
#define UMBRAL_TIEMPO_PUERTA_CERRADA 15000
#define DOOR_OPEN 1
#define DOOR_CLOSED 0

class UltrasonicDoor : public Ultrasonic
{
private:
    int timeOpened;
    int timeClosed;
    bool status;

    void startTimerOpened()
    {
        timeOpened = millis();
    }

    void startTimerClosed()
    {
        timeClosed = millis();
    }

    bool reachedTimeoutOpened()
    {
        int currentTime = millis();
        int timeElapsed = currentTime - timeOpened;
        return timeElapsed > UMBRAL_TIEMPO_PUERTA_ABIERTA;
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
    }

    bool checkStatus()
    {
        int currentDistance = getDistance();
        int previousDistance = this->previousDistance;
        //Serial.println("Se esta validando el sensor de la puerta");
        if (currentDistance != previousDistance)
        {
            this->previousDistance = currentDistance;
            bool isDoorDetected = currentDistance < UMBRAL_PUERTA_ABIERTA;
            if (isDoorDetected == false && status == DOOR_CLOSED)
            {
                //Serial.println("Se abrio la puerta");
                status = DOOR_OPEN;
                event = EVENTO_SE_ABRIO_PUERTA;
                return true;
            }
            else if (isDoorDetected == true && status == DOOR_OPEN)
            {
                //Serial.println("Se inicio contador puerta cerrada");
                startTimerClosed();
            }
        }
        // if (status == DOOR_OPEN && reachedTimeoutOpened())
        // {
        //     status = DOOR_CLOSED;
        //     event = EVENTO_SE_CERRO_PUERTA;
        //     return true;
        // }
        return false;
    }
};