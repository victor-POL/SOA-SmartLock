#include "main.h"

// Sensors
UltrasonicEntrance entranceSensor = UltrasonicEntrance(ENTRANCE_SENSOR_TRIGGER_PIN, ENTRANCE_SENSOR_ECHO_PIN);
UltrasonicDoor doorSensor = UltrasonicDoor(DOOR_SENSOR_TRIGGER_PIN, DOOR_SENSOR_ECHO_PIN);
Photoresistor lightSensor = Photoresistor(PHOTORESISTOR_PIN);
KeyPad keypad = KeyPad();

// Actuators
MyServo entranceDoor = MyServo(SERVO_PIN);
Buzzer buzzer = Buzzer(BUZZER_PIN);
Relay light = Relay(RELAY_PIN);
LCD *LCD::instance = NULL;
LCD lcd = *LCD::getInstance();

// Lock
Lock doorLock = Lock();

// Global
int state;
int event;
bool timeout;
long lastCurrentTime;

void doInit()
{
    Serial.begin(115200);

    entranceDoor.setup();
    entranceSensor.setup();
    doorSensor.setup();
    
    buzzer.setup();
    light.setup();
    lcd.setup();

    state = ESTADO_CERRADURA_INIT;
    event = EVENTO_CONTINUE;

    timeout = false;
    lastCurrentTime = millis();
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

void stateMachine()
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
}

void setup()
{
    doInit();
}

void loop()
{
    stateMachine();
}

void showActualState(String strState, String strEvent)
{
    Serial.println("-----------------------------------------------------");
    Serial.println("Estado: " + String(strState));
    Serial.println("Evento: " + String(strEvent));
    Serial.println("-----------------------------------------------------");
}

