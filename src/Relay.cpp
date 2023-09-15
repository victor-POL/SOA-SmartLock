#include <Arduino.h>
#define RELAY_PIN 25

class Relay
{
private:
    bool isOn;
public:
    Relay()
    {
        this->isOn = false;
    }

    void setup()
    {
        pinMode(RELAY_PIN, OUTPUT);
    }

    void turnOn()
    {
        isOn = true;;
        digitalWrite(RELAY_PIN, HIGH);
    }

    void turnOff()
    {
        isOn = false;
        digitalWrite(RELAY_PIN, LOW);
    }

    bool getIsOn()
    {
        return this->isOn;
    }
};