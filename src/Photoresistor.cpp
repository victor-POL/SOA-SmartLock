#include <Arduino.h>
#include "Component.cpp"

#define UMBRAL_LUX_NOCHE 200

class Photoresistor : public Component
{
private:
  const float GAMMA = 0.7;
  const float RL10 = 85;
  int previousLux;

public:
  Photoresistor(int pinSelected) : Component(pinSelected)
  {
  }

  void setup()
  {
    pinMode(this->pinSelected, INPUT);
  }

  int getLux()
  {
    int analogValue = analogRead(this->pinSelected);
    float voltage = analogValue / 1024. * 3.3;
    float resistance = 5000 * voltage / (1 - voltage / 3.3);
    float lux = pow(RL10 * 1e3 * pow(10, this->GAMMA) / resistance, (1 / this->GAMMA));
    this->previousLux = lux;
    return lux;
  }

  String getStatus()
  {
    return getLux() > UMBRAL_LUX_NOCHE ? "DAY" : "NIGHT";
  }
};