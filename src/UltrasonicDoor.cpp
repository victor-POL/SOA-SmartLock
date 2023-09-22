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
  bool doorOpenNotificationSent;
  bool timerOpenedActivated;
  bool timerClosedActivated;
  bool status;

  void StartTimerOpened()
  {
    timeOpened = millis();
  }

  void StartTimerClosed()
  {
    timeClosed = millis();
  }

  bool ReachedTimeoutClosed()
  {
    int currentTime = millis();
    int timeElapsed = currentTime - timeClosed;
    return timeElapsed > UMBRAL_TIEMPO_PUERTA_CERRADA;
  }

  bool ReachedTimeoutOpened()
  {
    int currentTime = millis();
    int timeElapsed = currentTime - timeOpened;
    return timeElapsed > UMBRAL_TIEMPO_PUERTA_ABIERTA;
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

  bool CheckStatus()
  {
    int currentDistance = GetDistance();
    int previousDistance = this->previousDistance;

    if (timerClosedActivated == true && ReachedTimeoutClosed())
    {
      timerClosedActivated = false;
      doorOpenNotificationSent = false;
      status = DOOR_CLOSED;
      event = EVENTO_SE_CERRO_PUERTA;
      return true;
    }

    if (timerOpenedActivated == true && doorOpenNotificationSent == true && ReachedTimeoutOpened() == true)
    {
      doorOpenNotificationSent = false;
      event = EVENTO_NOTIFICAR_PUERTA_ABIERTA;
      return true;
    }

    if (currentDistance != previousDistance)
    {
      this->previousDistance = currentDistance;
      bool isDoorDetected = currentDistance < UMBRAL_PUERTA_ABIERTA;

      if (isDoorDetected == false && status == DOOR_CLOSED)
      {
        doorOpenNotificationSent = true;
        timerOpenedActivated = true;
        StartTimerOpened();
        status = DOOR_OPEN;
        event = EVENTO_SE_ABRIO_PUERTA;
        return true;
      }
      else if (status == DOOR_OPEN)
      {
        if (isDoorDetected == true && timerClosedActivated == false)
        {
          StartTimerClosed();
          timerClosedActivated = true;
        }
        else if (isDoorDetected == false && timerClosedActivated == true)
        {
          timerClosedActivated = false;
        }
      }
    }

    return false;
  }
};