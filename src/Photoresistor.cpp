#include <Arduino.h>
#include "Component.cpp"



class Photoresistor : public Component
{
private:
    const float GAMMA = 0.7;
    const float RL10 = 85;

    float previousLux;

public:
    Photoresistor(int pinSelected) : Component(pinSelected)
    {
        this->previousLux = 0;
    }

    void setup()
    {
        pinMode(this->pinSelected, INPUT);
    }

    int getLight()
    {
        int analogValue = analogRead(this->pinSelected);
        float voltage = analogValue / 1024. * 3.3;
        float resistance = 5000 * voltage / (1 - voltage / 3.3);
        float lux = pow(RL10 * 1e3 * pow(10, this->GAMMA) / resistance, (1 / this->GAMMA));
        return lux;
    }
};