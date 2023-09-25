#include <ESP32Servo.h>

#include "KeyPad.cpp"
#include "Buzzer.cpp"
#include "MyServo.cpp"
#include "Lock.cpp"
#include "UltrasonicEntrance.cpp"
#include "UltrasonicDoor.cpp"
#include "Relay.cpp"
#include "Button.cpp"

#include "States.h"
#include "Events.h"
#include "Connections.h"

#define COMPILAR_PARA_SIMULADOR true
#if COMPILAR_PARA_SIMULADOR
#include "LCD.cpp"
#else
#include "LCDRgb.cpp"
#endif

#define UMBRAL_DIFERENCIA_TIMEOUT 50

#define SERIAL_DEBUG_ENABLED 1

#if SERIAL_DEBUG_ENABLED
  #define DebugPrint(str)\
      {\
        Serial.println(str);\
      }
#else
  #define DebugPrint(str)
#endif

#define DebugPrintEstado(estado,evento)\
      {\
        String est = estado;\
        String evt = evento;\
        String str;\
        str = "-----------------------------------------------------";\
        DebugPrint(str);\
        str = "EST-> [" + est + "]: " + "EVT-> [" + evt + "].";\
        DebugPrint(str);\
        str = "-----------------------------------------------------";\
        DebugPrint(str);\
      }

typedef void (*transition)();