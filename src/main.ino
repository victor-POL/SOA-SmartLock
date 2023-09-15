#include "main.h"

LCD *LCD::instance = NULL;
LCD lcd = *LCD::getInstance();
UltrasonicEntrance entranceSensor = UltrasonicEntrance(TRIGGER_PIN_SENSOR_1, ECHO_PIN_SENSOR_1);
UltrasonicDoor doorSensor = UltrasonicDoor(TRIGGER_PIN_SENSOR_2, ECHO_PIN_SENSOR_2);
Relay light = Relay();
Photoresistor lightSensor = Photoresistor();
KeyPad keypad = KeyPad();
Lock doorLock = Lock();
MyServo entranceDoor = MyServo();
Buzzer buzzer = Buzzer();

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
        buzzer.checkStatus();
        if (entranceSensor.checkStatus() || keypad.checkStatus() || doorLock.checkStatus() || doorSensor.checkStatus())
        {
            return;
        }
    }

    event = EVENTO_CONTINUE;
}

void setup()
{
    Serial.begin(115200);
    entranceDoor.setup();
    lcd.setup();
    entranceSensor.setup();
    doorSensor.setup();
    light.setup();
    buzzer.setup();
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
    break;

    case ESTADO_BLOQUEADO_ESPERANDO_VISITA:
    {
        switch (event)
        {
        case EVENTO_PERSONA_DETECTADA:
        {
            showActualState("ESTADO_BLOQUEADO_ESPERANDO_VISITA", "EVENTO_PERSONA_DETECTADA");
            lcd.turnOn();
            lcd.resetInputPassScreen();
            if (lightSensor.getLight() < UMBRAL_LUZ_APAGADA)
            {
                light.turnOn();
            }
            state = ESTADO_ESPERANDO_INGRESO_CONTRASENA;
        }
        break;

        case EVENTO_PERSONA_NO_DETECTADA:
        {
            showActualState("ESTADO_BLOQUEADO_ESPERANDO_VISITA", "EVENTO_PERSONA_NO_DETECTADA");
            state = ESTADO_BLOQUEADO_ESPERANDO_VISITA;
        }
        break;

        case EVENTO_CONTINUE:
        {
            state = ESTADO_BLOQUEADO_ESPERANDO_VISITA;
        }
        break;
        }
    }
    break;

    case ESTADO_ESPERANDO_INGRESO_CONTRASENA:
    {
        switch (event)
        {
        case EVENTO_PERSONA_NO_DETECTADA:
        {
            showActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_PERSONA_NO_DETECTADA");
            lcd.turnOff();
            light.turnOff();
            doorLock.resetPassEntered();
            state = ESTADO_BLOQUEADO_ESPERANDO_VISITA;
        }
        break;

        case EVENTO_VALIDAR_CLAVE:
        {
            showActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_VALIDAR_CLAVE");
            buzzer.activateKeyPressedSound();
            doorLock.changeUnlockInProgress(true);
            state = ESTADO_VALIDACION_CLAVE;
        }
        break;

        case EVENTO_PERSONA_DETECTADA:
        {
            showActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_PERSONA_DETECTADA");
            state = ESTADO_ESPERANDO_INGRESO_CONTRASENA;
        }
        break;

        case EVENTO_CARACTER_INGRESADO:
        {
            showActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_CARACTER_INGRESADO");
            buzzer.activateKeyPressedSound();
            doorLock.loadCharacter(keypad.getLastKeyPressed());
            lcd.showKeyPressed(keypad.getLastKeyPressed());
            state = ESTADO_ESPERANDO_INGRESO_CONTRASENA;
        }
        break;

        case EVENTO_CLEAR_CLAVE_INGRESADA:
        {
            showActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_CLEAR_CLAVE_INGRESADA");
            buzzer.activateKeyPressedSound();
            lcd.resetInputPassScreen();
            doorLock.resetPassEntered();
            state = ESTADO_ESPERANDO_INGRESO_CONTRASENA;
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
    {
        switch (event)
        {
        case EVENTO_TIMEOUT_VALIDACION_CLAVE:
        {
            showActualState("ESTADO_VALIDACION_CLAVE", "EVENTO_TIMEOUT");
            lcd.showTimeoutMessage();
            buzzer.activateErrorSound();
            state = ESTADO_ESPERANDO_INGRESO_CONTRASENA;
        }
        break;

        case EVENTO_CLAVE_VALIDA:
        {
            showActualState("ESTADO_VALIDACION_CLAVE", "EVENTO_CLAVE_VALIDA");
            lcd.showValidPassMessage();
            buzzer.activateSuccessSound();
            entranceDoor.unlock();
            state = ESTADO_ESPERANDO_APERTURA_PUERTA;
        }
        break;

        case EVENTO_CLAVE_INVALIDA:
        {
            showActualState("ESTADO_VALIDACION_CLAVE", "EVENTO_CLAVE_INVALIDA");
            lcd.showInvalidPassMessage();
            buzzer.activateErrorSound();
            lcd.resetInputPassScreen();
            doorLock.resetPassEntered();
            state = ESTADO_ESPERANDO_INGRESO_CONTRASENA;
        }
        break;

        case EVENTO_CONTINUE:
        {
            state = ESTADO_VALIDACION_CLAVE;
        }
        break;
        }
    }
    break;

    case ESTADO_ESPERANDO_APERTURA_PUERTA:
    {
        switch (event)
        {
        case TIMEOUT_APERTURA_PUERTA:
        {
            showActualState("ESTADO_ESPERANDO_ENTRADA_PERSONA", "TIMEOUT_APERTURA_PUERTA");
            entranceDoor.lock();
            lcd.resetInputPassScreen();
            doorLock.lock();
            state = ESTADO_ESPERANDO_INGRESO_CONTRASENA;
        }
        break;

        case EVENTO_PUERTA_ABIERTA:
        {
            showActualState("ESTADO_ESPERANDO_ENTRADA_PERSONA", "EVENTO_PUERTA_ABIERTA");
            state = ESTADO_ESPERANDO_ENTRADA_PERSONA;
        }
        break;

        case EVENTO_CONTINUE:
        {
            state = ESTADO_ESPERANDO_APERTURA_PUERTA;
        }
        break;
        }
    }
    break;

    case ESTADO_ESPERANDO_ENTRADA_PERSONA:
        break;
    }

    event = EVENTO_CONTINUE;

    delay(10);
}