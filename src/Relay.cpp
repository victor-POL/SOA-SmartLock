#include <Arduino.h>
#include "Component.cpp"

class Relay : public Component
{
private:
  bool is_on;

public:
  Relay(int pin_selected) : Component(pin_selected)
  {
    this->is_on = false;
  }

  void Setup()
  {
    pinMode(this->pin_selected, OUTPUT);
  }

  void TurnOn()
  {
    this->is_on = true;
    digitalWrite(this->pin_selected, HIGH);
  }

  void TurnOff()
  {
    this->is_on = false;
    digitalWrite(this->pin_selected, LOW);
  }

  bool get_is_on()
  {
    return this->is_on;
  }
};