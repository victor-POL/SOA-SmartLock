#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>
#include "States.h"
#include "Events.h"
#include "Connections.h"

#define UMBRAL_PERSONA_DETECTADA 50
#define UMBRAL_PUERTA_ABIERTA 20

extern enum Events event;

class Ultrasonic
{
protected:
  int trigger_pin_selected;
  int echo_pin_selected;
  int previous_distance;

public:
  Ultrasonic(int echo_pin, int trigger_pin)
  {
    this->trigger_pin_selected = echo_pin;
    this->echo_pin_selected = trigger_pin;
    this->previous_distance = 0;
  }

  void Setup()
  {
    pinMode(trigger_pin_selected, OUTPUT);
    pinMode(echo_pin_selected, INPUT);
  }

  int GetDistance()
  {
    digitalWrite(trigger_pin_selected, LOW);
    delayMicroseconds(2);

    digitalWrite(trigger_pin_selected, HIGH);
    delayMicroseconds(10);

    digitalWrite(trigger_pin_selected, LOW);

    return 0.01723 * pulseIn(echo_pin_selected, HIGH);
  }
};

#endif