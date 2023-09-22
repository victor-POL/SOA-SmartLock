#include <Arduino.h>
#include "Component.cpp"
#include "States.h"
#include "Events.h"
#include "Connections.h"

extern enum Event event;

class Button : public Component
{
private:
  int ReadSensor()
  {
    return digitalRead(this->pin_selected);
  }

public:
  Button(int pin_selected) : Component(pin_selected)
  {
  }

  void Setup()
  {
    pinMode(this->pin_selected, INPUT);
  }

  bool CheckStatus()
  {
    if (ReadSensor() == LOW)
    {
      event = Event::BotonPresionado;
      return true;
    }
    return false;
  }
};