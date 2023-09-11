#include <Arduino.h>

#define LIGHT_SEN_PIN 39

class Photoresistor
{
private:
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
        return analogRead(LIGHT_SEN_PIN);
    }
};