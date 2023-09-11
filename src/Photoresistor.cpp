#include <Arduino.h>

#define LIGHT_SEN_PIN 39

class Photoresistor
{
private:
    const float GAMMA = 0.7;
    const float RL10 = 85;

    float previousLux;

public:
    Photoresistor()
    {
    }

    void setup()
    {
        pinMode(LIGHT_SEN_PIN, INPUT);
    }

    int getLight()
    {
        int analogValue = analogRead(LIGHT_SEN_PIN);
        float voltage = analogValue / 1024. * 3.3;
        float resistance = 5000 * voltage / (1 - voltage / 3.3);
        float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));
        return lux;
    }
};