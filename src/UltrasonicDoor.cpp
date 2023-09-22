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
  bool door_open_notification_sent;
  bool timer_opened_activated;
  bool timer_closed_activated;
  bool status;

  void StartTimerOpened()
  {
    time_opened = millis();
  }

  void StartTimerClosed()
  {
    time_closed = millis();
  }

  bool ReachedTimeoutClosed()
  {
    int current_time = millis();
    int time_elapsed = current_time - time_closed;
    return time_elapsed > UMBRAL_TIEMPO_PUERTA_CERRADA;
  }

  bool ReachedTimeoutOpened()
  {
    int current_time = millis();
    int time_elapsed = current_time - time_opened;
    return time_elapsed > UMBRAL_TIEMPO_PUERTA_ABIERTA;
  }

public:
  UltrasonicDoor(int trigger_pin, int echo_pin) : Ultrasonic(trigger_pin, echo_pin)
  {
    previous_distance = 0;
    time_opened = 0;
    time_closed = 0;
    status = DOOR_CLOSED;
    timer_closed_activated = false;
  }

  bool CheckStatus()
  {
    int current_distance = GetDistance();
    int previous_distance = this->previous_distance;

    if (timer_closed_activated == true && ReachedTimeoutClosed())
    {
      timer_closed_activated = false;
      door_open_notification_sent = false;
      status = DOOR_CLOSED;
      event = Event::SeCerroPuerta;
      return true;
    }

    if (timer_closed_activated == true && door_open_notification_sent == true && ReachedTimeoutOpened() == true)
    {
      door_open_notification_sent = false;
      event = Event::NotificarPuertaAbierta;
      return true;
    }

    if (current_distance != previous_distance)
    {
      this->previous_distance = current_distance;
      bool is_door_detected = current_distance < UMBRAL_PUERTA_ABIERTA;

      if (is_door_detected == false && status == DOOR_CLOSED)
      {
        door_open_notification_sent = true;
        timer_opened_activated = true;
        StartTimerOpened();
        status = DOOR_OPEN;
        event = Event::SeAbrioPuerta;
        return true;
      }
      else if (status == DOOR_OPEN)
      {
        if (is_door_detected == true && timer_closed_activated == false)
        {
          StartTimerClosed();
          timer_closed_activated = true;
        }
        else if (is_door_detected == false && timer_closed_activated == true)
        {
          timer_closed_activated = false;
        }
      }
    }

    return false;
  }
};