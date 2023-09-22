#include <Arduino.h>
#include "Component.cpp"

class Relay : public Component
{
private:
  bool isOn;

public:
  Relay(int pin_selected) : Component(pin_selected)
  {
    this->isOn = false;
  }

  void Setup()
  {
    pinMode(this->pin_selected, OUTPUT);
  }

  void TurnOn()
  {
    this->isOn = true;
    digitalWrite(this->pin_selected, HIGH);
  }

  void TurnOff()
  {
    this->isOn = false;
    digitalWrite(this->pin_selected, LOW);
  }

  bool get_is_on()
  {
    return this->isOn;
  }
};