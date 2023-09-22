#include <Arduino.h>
#include "States.h"
#include "Events.h"
#include "Connections.h"

extern enum Event event;

class Button
{
private:
public:
  Button()
  {
  }

  void Setup()
  {
    pinMode(BUTTON_PIN, INPUT);
  }

  bool CheckStatus()
  {
    if (digitalRead(BUTTON_PIN) == LOW)
    {
      event = Event::BotonPresionado;
      return true;
    }
    return false;
  }
};