#ifndef COMPONENT_H
#define COMPONENT_H

#include <Arduino.h>

class Component
{
protected:
  int pin_selected;

public:
  Component(int pin_selected)
  {
    this->pin_selected = pin_selected;
  }
};

#endif