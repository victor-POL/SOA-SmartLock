#include <Arduino.h>
#include "Component.cpp"
#include "States.h"
#include "Events.h"
#include "Connections.h"

#define BUTTON_PRESSED LOW

extern enum Event event;

class Button : public Component
{
private:
  int previous_value = 0;

  int ReadSensor()
  {
    return digitalRead(this->pin_selected);
  }

public:
  Button(int pin_selected) : Component(pin_selected)
  {
    this->previous_value = ReadSensor();
  }

  void Setup()
  {
    pinMode(this->pin_selected, INPUT);
  }

  bool CheckStatus()
  {
    int actual_value = ReadSensor();
    if (actual_value != previous_value)
    {
      previous_value = actual_value;
      if (ReadSensor() == BUTTON_PRESSED)
      {
        event = Event::BotonPresionado;
        return true;
      }
    }

    return false;
  }
};