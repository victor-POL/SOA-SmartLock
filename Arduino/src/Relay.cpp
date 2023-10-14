#include <Arduino.h>
#include "Component.cpp"

class Relay : public Component
{
private:
  bool is_on;

  void WriteValue(int value)
  {
    digitalWrite(this->pin_selected, value);
  }

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
    WriteValue(HIGH);
  }

  void TurnOff()
  {
    this->is_on = false;
    WriteValue(LOW);
  }

  bool GetIsOn()
  {
    return this->is_on;
  }
};