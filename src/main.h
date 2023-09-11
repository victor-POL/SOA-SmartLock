#include <ESP32Servo.h>

#include "KeyPad.cpp"
#include "LCD.cpp"
#include "Buzzer.cpp"
#include "Servo.cpp"
#include "Lock.cpp"
#include "Ultrasonic.cpp"
#include "Relay.cpp"
#include "Photoresistor.cpp"

#include "States.h"
#include "Events.h"

#define TRIGGER_PIN_SENSOR_1                        32
#define TRIGGER_PIN_SENSOR_2                        33
#define ECHO_PIN_SENSOR_1                           34
#define ECHO_PIN_SENSOR_2                           35

#define UMBRAL_DIFERENCIA_TIMEOUT                   50