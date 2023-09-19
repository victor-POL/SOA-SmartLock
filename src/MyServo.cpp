#include <Arduino.h>
#include <ESP32Servo.h>
#include "Component.cpp"
#define MIN_ANGLE 500
#define MAX_ANGLE 2500

class MyServo : public Component
{
private:
    Servo servo;

public:
    MyServo(int pinSelected) : Component(pinSelected)
    {
        this->servo = Servo();
    }

    void setup()
    {
        this->servo.attach(this->pinSelected, MIN_ANGLE, MAX_ANGLE);
        this->servo.write(0);
    }

    void changeOrientation(int angle)
    {
        this->servo.write(angle);
    }

    void unlock()
    {
        this->servo.write(179);
    }

    void lock()
    {
        this->servo.write(0);
    }
};
