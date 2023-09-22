#include <Arduino.h>
#include "Component.cpp"

class Relay : public Component
{
private:
  bool isOn;

public:
  Relay(int pinSelected) : Component(pinSelected)
  {
    this->isOn = false;
  }

  void Setup()
  {
    pinMode(this->pinSelected, OUTPUT);
  }

  void TurnOn()
  {
    this->isOn = true;
    digitalWrite(this->pinSelected, HIGH);
  }

  void TurnOff()
  {
    this->isOn = false;
    digitalWrite(this->pinSelected, LOW);
  }

  bool get_is_on()
  {
    return this->isOn;
  }
};