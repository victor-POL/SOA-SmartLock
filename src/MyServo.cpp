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

  void Setup()
  {
    this->servo.attach(this->pin_selected, MIN_ANGLE, MAX_ANGLE);
    this->servo.write(LOCK_ANGLE);
  }

  void ChangeOrientation(int angle)
  {
    this->servo.write(angle);
  }

  void Unlock()
  {
    this->servo.write(UNLOCK_ANGLE);
  }

  void Lock()
  {
    this->servo.write(LOCK_ANGLE);
  }
};
