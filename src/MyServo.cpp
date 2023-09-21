#include <Arduino.h>
#include <ESP32Servo.h>
#include "Component.cpp"
#define MIN_ANGLE 500
#define MAX_ANGLE 2500
#define LOCK_ANGLE 0
#define UNLOCK_ANGLE 179

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
    this->servo.write(LOCK_ANGLE);
  }

  void changeOrientation(int angle)
  {
    this->servo.write(angle);
  }

  void unlock()
  {
    this->servo.write(UNLOCK_ANGLE);
  }

  void lock()
  {
    this->servo.write(LOCK_ANGLE);
  }
};
