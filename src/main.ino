#include "main.h"

LCD *LCD::instance = NULL;
LCD lcd = *LCD::getInstance();
Ultrasonic entranceSensor = Ultrasonic(TRIGGER_PIN_SENSOR_1, ECHO_PIN_SENSOR_1);

int state;
int event;
bool timeout;
long lastCurrentTime;

void showActualState(String strState, String strEvent)
{
    Serial.println("-----------------------------------------------------");
    Serial.println("Estado: " + String(strState));
    Serial.println("Evento: " + String(strEvent));
    Serial.println("-----------------------------------------------------");
}

void generateEvent()
{
    long currentTime = millis();
    int diff = currentTime - lastCurrentTime;
    timeout = (diff > UMBRAL_DIFERENCIA_TIMEOUT) ? true : false;

    if (timeout)
    {
        timeout = false;
        lastCurrentTime = currentTime;

        if (entranceSensor.checkStatus())
        {
            return;
        }
    }

    event = EVENTO_CONTINUE;
}

void setup()
{
    Serial.begin(115200);

    lcd.setup();

    entranceSensor.setup();

    state = ESTADO_CERRADURA_INIT;
    event = EVENTO_CONTINUE;
    timeout = false;
    lastCurrentTime = millis();
}

void loop()
{
    generateEvent();

    switch (state)
    {
    case ESTADO_CERRADURA_INIT:
    {
        switch (event)
        {
        case EVENTO_CONTINUE:
        {
            showActualState("ESTADO_CERRADURA_INIT", "EVENTO_CONTINUE");

            lcd.turnOff();

            state = ESTADO_BLOQUEADO_ESPERANDO_VISITA;
        }
        break;
        }
    }
    case ESTADO_BLOQUEADO_ESPERANDO_VISITA:
    {
        switch (event)
        {
        case EVENTO_PERSONA_DETECTADA:
        {
            showActualState("ESTADO_BLOQUEADO_ESPERANDO_VISITA", "EVENTO_PERSONA_DETECTADA");

            lcd.turnOn();
            lcd.setupInputPassScreen();

            state = ESTADO_ESPERANDO_INGRESO_CONTRASENA;
        }
        break;
        case EVENTO_PERSONA_NO_DETECTADA:
        {
            showActualState("ESTADO_BLOQUEADO_ESPERANDO_VISITA", "EVENTO_PERSONA_NO_DETECTADA");
        }
        break;
        case EVENTO_CONTINUE:
            state = ESTADO_BLOQUEADO_ESPERANDO_VISITA;
            break;
        }
    }
    break;
    case ESTADO_ESPERANDO_INGRESO_CONTRASENA:
    {
        switch (event)
        {
        case EVENTO_PERSONA_SE_FUE:
        {
            showActualState("ESTADO_BLOQUEADO_ESPERANDO_VISITA", "EVENTO_PERSONA_SE_FUE");

            lcd.turnOff();

            state = ESTADO_BLOQUEADO_ESPERANDO_VISITA;
        }
        break;
        case EVENTO_CONTINUE:
        {
            state = ESTADO_ESPERANDO_INGRESO_CONTRASENA;
        }
        break;
        }
    }
    break;
    case ESTADO_VALIDACION_CLAVE:
        break;
    case ESTADO_ESPERANDO_APERTURA_PUERTA:
        break;
    case ESTADO_ESPERANDO_ENTRADA_PERSONA:
        break;
    }

    delay(10);
}