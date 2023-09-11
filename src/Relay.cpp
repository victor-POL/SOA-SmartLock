#include <Arduino.h>
#define RELAY_PIN 14

class Relay
{
private:
public:
    Relay()
    {
    }

    void setup()
    {
        pinMode(RELAY_PIN, OUTPUT);
    }

    void turnOn()
    {
        digitalWrite(RELAY_PIN, HIGH);
    }

    void turnOff()
    {
        digitalWrite(RELAY_PIN, LOW);
    }
};