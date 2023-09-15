#include <ESP32Servo.h>

#include "KeyPad.cpp"
#include "LCD.cpp"
#include "Buzzer.cpp"
#include "MyServo.cpp"
#include "Lock.cpp"
#include "UltrasonicEntrance.cpp"
#include "UltrasonicDoor.cpp"
#include "Relay.cpp"

#include "States.h"
#include "Events.h"
#include "Connections.h"

#define UMBRAL_DIFERENCIA_TIMEOUT 50