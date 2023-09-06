#include <Arduino.h>
#include <ESP32Servo.h>

#define SERVO_PIN 12

class MyServo
{
private:
    Servo servo = Servo();

public:
    MyServo()
    {
    }

    void setup()
    {
        this->servo.attach(SERVO_PIN, 500, 2500);
        this->servo.write(0);
    }

    void changeOrientation(int angle)
    {
        servo.write(angle);
    }
};