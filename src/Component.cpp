#ifndef COMPONENT_H
#define COMPONENT_H

#include <Arduino.h>

class Component
{
protected:
  int pinSelected;

public:
  Component(int pinSelected)
  {
    this->pinSelected = pinSelected;
  }
};

#endif