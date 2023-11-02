#include <Arduino.h>
#include <ESP32Servo.h>
#include "Component.cpp"
#define MIN_ANGLE 500
#define MAX_ANGLE 2500
#define LOCK_ANGLE 90
#define UNLOCK_ANGLE 20

class MyServo : public Component
{
private:
  Servo servo;

  void ChangeOrientation(int angle)
  {
    this->servo.write(angle);
  }

public:
  MyServo(int pin_selected) : Component(pin_selected)
  {
    this->servo = Servo();
  }

  void Setup()
  {
    this->servo.attach(this->pin_selected, MIN_ANGLE, MAX_ANGLE);
    this->servo.write(LOCK_ANGLE);
  }

  void Unlock()
  {
    ChangeOrientation(UNLOCK_ANGLE);
  }

  void Lock()
  {
    ChangeOrientation(LOCK_ANGLE);
  }
};
