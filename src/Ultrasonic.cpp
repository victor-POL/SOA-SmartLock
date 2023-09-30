#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>
#include "States.h"
#include "Events.h"
#include "Connections.h"

extern enum Event event;

class Ultrasonic
{
protected:
  int trigger_pin_selected;
  int echo_pin_selected;
  int previous_distance;

  int ReadSensor()
  {
    digitalWrite(this->trigger_pin_selected, LOW);
    delayMicroseconds(2);

    digitalWrite(this->trigger_pin_selected, HIGH);
    delayMicroseconds(10);

    digitalWrite(this->trigger_pin_selected, LOW);

    return pulseIn(this->echo_pin_selected, HIGH);
  }

public:
  Ultrasonic(int echo_pin, int trigger_pin)
  {
    this->trigger_pin_selected = echo_pin;
    this->echo_pin_selected = trigger_pin;
    this->previous_distance = GetDistance();
  }

  void Setup()
  {
    pinMode(this->trigger_pin_selected, OUTPUT);
    pinMode(this->echo_pin_selected, INPUT);
  }

  int GetDistance()
  {
    return 0.01723 * ReadSensor();
  }
};

#endif