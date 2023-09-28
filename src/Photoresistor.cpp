#include <Arduino.h>
#include "Component.cpp"

#define UMBRAL_LUX_NOCHE 2000

class Photoresistor : public Component
{
private:
  const float GAMMA = 0.7;
  const float RL10 = 85;
  int previous_lux;
  
  int ReadSensor()
  {
    return analogRead(this->pin_selected);
  }

public:
  Photoresistor(int pin_selected) : Component(pin_selected)
  {
  }

  void Setup()
  {
    pinMode(this->pin_selected, INPUT);
  }

  int GetLux()
  {
    int analog_value = ReadSensor();
    return analog_value;
  }

  String GetStatus()
  {
    return GetLux() > UMBRAL_LUX_NOCHE ? "DAY" : "NIGHT";
  }
};