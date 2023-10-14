#include <ESP32Servo.h>

#include "KeyPad.cpp"
#include "Buzzer.cpp"
#include "MyServo.cpp"
#include "Lock.cpp"
#include "UltrasonicEntrance.cpp"
#include "UltrasonicDoor.cpp"
#include "Relay.cpp"
#include "Button.cpp"
#include "NFC.cpp"
#include "MyWifi.h"

#include "States.h"
#include "Events.h"
#include "Connections.h"
#include "Config.h"

#if COMPILAR_PARA_SIMULADOR
#include "LCD.cpp"
#else
#include "LCDRgb.cpp"
#endif

#define UMBRAL_DIFERENCIA_TIMEOUT 50

typedef void (*transition)();