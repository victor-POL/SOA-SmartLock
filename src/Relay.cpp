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

  void setup()
  {
    pinMode(this->pinSelected, OUTPUT);
  }

  void turnOn()
  {
    this->isOn = true;
    digitalWrite(this->pinSelected, HIGH);
  }

  void turnOff()
  {
    this->isOn = false;
    digitalWrite(this->pinSelected, LOW);
  }

  bool getIsOn()
  {
    return this->isOn;
  }
};