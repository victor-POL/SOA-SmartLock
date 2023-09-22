#include <Arduino.h>
#include "Component.cpp"

#define UMBRAL_LUX_NOCHE 200

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
    float voltage = analog_value / 1024. * 3.3;
    float resistance = 5000 * voltage / (1 - voltage / 3.3);
    float lux = pow(this->RL10 * 1e3 * pow(10, this->GAMMA) / resistance, (1 / this->GAMMA));
    this->previous_lux = lux;
    return lux;
  }

  String GetStatus()
  {
    return GetLux() > UMBRAL_LUX_NOCHE ? "DAY" : "NIGHT";
  }
};