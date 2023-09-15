#include <Arduino.h>
#include <ESP32Servo.h>
#include "Component.cpp"

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
        this->servo.attach(this->pinSelected, 500, 2500);
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