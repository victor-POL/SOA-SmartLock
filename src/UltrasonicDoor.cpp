#include "Ultrasonic.cpp"
#define UMBRAL_TIEMPO_PUERTA_ABIERTA 10000
#define UMBRAL_TIEMPO_PUERTA_CERRADA 5000
#define DOOR_OPEN 1
#define DOOR_CLOSED 0

class UltrasonicDoor : public Ultrasonic
{
private:
  int time_opened;
  int time_closed;
  bool timer_opened_activated;
  bool timer_closed_activated;
  bool doorStatus;

  void StartTimerOpened()
  {
    this->time_opened = millis();
    this->timer_opened_activated = true;
  }

  void StartTimerClosed()
  {
    this->time_closed = millis();
    this->timer_closed_activated = true;
  }

  void StopTimerOpened()
  {
    this->timer_opened_activated = false;
  }

  void StopTimerClosed()
  {
    this->timer_closed_activated = false;
  }

  bool ReachedTimeoutClosed()
  {
    int current_time = millis();
    int time_elapsed = current_time - this->time_closed;
    return time_elapsed > UMBRAL_TIEMPO_PUERTA_CERRADA;
  }

  bool ReachedTimeoutOpened()
  {
    int current_time = millis();
    int time_elapsed = current_time - this->time_opened;
    return time_elapsed > UMBRAL_TIEMPO_PUERTA_ABIERTA;
  }

public:
  UltrasonicDoor(int trigger_pin, int echo_pin) : Ultrasonic(trigger_pin, echo_pin)
  {
    this->previous_distance = 0;
    this->time_opened = 0;
    this->time_closed = 0;
    this->doorStatus = DOOR_CLOSED;
    this->timer_closed_activated = false;
  }

  // Status
  bool CheckClosedDoorTimer()
  {
    if (timer_closed_activated == true && ReachedTimeoutClosed())
    {
      this->timer_closed_activated = false;
      this->doorStatus = DOOR_CLOSED;
      event = Event::SeCerroPuerta;
      return true;
    }
    return false;
  }

  bool CheckOpenedDoorTimer()
  {
    if (timer_opened_activated == true && ReachedTimeoutOpened() == true)
    {
      this->timer_opened_activated = false;
      event = Event::NotificarPuertaAbierta;
      return true;
    }
    return false;
  }

  bool CheckStatus()
  {
    int current_distance = GetDistance();
    int previous_distance = this->previous_distance;

    if (abs(current_distance - previous_distance) > UMBRAL_CAMBIO_VALOR)
    {
      this->previous_distance = current_distance;
      bool is_door_detected = current_distance < UMBRAL_PUERTA_ABIERTA;

      if (this->doorStatus == DOOR_CLOSED && is_door_detected == false)
      {
        StartTimerOpened();
        StopTimerClosed();
        this->doorStatus = DOOR_OPEN;
        event = Event::SeAbrioPuerta;
        return true;
      }
      else if (this->doorStatus == DOOR_OPEN && is_door_detected == true)
      {
        StartTimerClosed();
        StopTimerOpened();
        this->doorStatus = DOOR_CLOSED;
      }
    }

    return false;
  }
};