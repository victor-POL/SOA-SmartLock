#include "main.h"

// Sensors
UltrasonicEntrance entranceSensor = UltrasonicEntrance(ENTRANCE_SENSOR_TRIGGER_PIN, ENTRANCE_SENSOR_ECHO_PIN);
UltrasonicDoor doorSensor = UltrasonicDoor(DOOR_SENSOR_TRIGGER_PIN, DOOR_SENSOR_ECHO_PIN);
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

        if (state == ESTADO_ESPERANDO_INGRESO_CONTRASENA && lcd.checkCursorInterval() == UPDATE_CURSOR)
        {
            lcd.updateCursor();
        }

        if (doorLock.checkStatus() || keypad.checkStatus() || doorSensor.checkStatus() || entranceSensor.checkStatus())
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
        case EVENTO_CLAVE_NO_CONFIGURADA:
        {
            showActualState("ESTADO_CERRADURA_INIT", "EVENTO_CLAVE_NO_CONFIGURADA");
            lcd.showMessageFullScreen("Clave no configurada");
            state = ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE;
        }
        break;
        case EVENTO_CONTINUE:
        {
            showActualState("ESTADO_CERRADURA_INIT", "EVENTO_CONTINUE");
            state = ESTADO_BLOQUEADO_ESPERANDO_VISITA;
        }
        break;
        }
    }
    break;

    case ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE:
    {
        switch (event)
        {
        }
    }
    break;

    case ESTADO_BLOQUEADO_ESPERANDO_VISITA:
    {
        switch (event)
        {
        case EVENTO_PERSONA_DETECTADA_DIA:
        {
            showActualState("ESTADO_BLOQUEADO_ESPERANDO_VISITA", "EVENTO_PERSONA_DETECTADA_DIA");
            initializeScreenToInputPassword();
            turnOffEntranceLight();
            state = ESTADO_ESPERANDO_INGRESO_CONTRASENA;
        }
        break;

        case EVENTO_PERSONA_DETECTADA_NOCHE:
        {
            showActualState("ESTADO_BLOQUEADO_ESPERANDO_VISITA", "EVENTO_PERSONA_DETECTADA_NOCHE");
            initializeScreenToInputPassword();
            turnOnEntranceLight();
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
            shutdownScreen();
            turnOffEntranceLight();
            clearPassEnteredIntoLock();
            state = ESTADO_BLOQUEADO_ESPERANDO_VISITA;
        }
        break;

        case EVENTO_VALIDAR_CLAVE:
        {
            showActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_VALIDAR_CLAVE");
            reproduceKeyPressedSoundInBuzzer();
            startPasswordValidation();
            state = ESTADO_VALIDACION_CLAVE;
        }
        break;

        case EVENTO_CARACTER_INGRESADO:
        {
            showActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_CARACTER_INGRESADO");
            reproduceKeyPressedSoundInBuzzer();
            showPasswordCharPressedOnScreen();
            loadPasswordCharPressedIntoLock();
            state = ESTADO_ESPERANDO_INGRESO_CONTRASENA;
        }
        break;

        case EVENTO_CLEAR_CLAVE_INGRESADA:
        {
            showActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_CLEAR_CLAVE_INGRESADA");
            reproduceKeyPressedSoundInBuzzer();
            clearPassEnteredOnScreen();
            clearPassEnteredIntoLock();
            state = ESTADO_ESPERANDO_INGRESO_CONTRASENA;
        }
        break;

        case EVENTO_PERSONA_DETECTADA_DIA:
        {
            showActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_PERSONA_DETECTADA_DIA");
            turnOffEntranceLight();
            state = ESTADO_ESPERANDO_INGRESO_CONTRASENA;
        }
        break;

        case EVENTO_PERSONA_DETECTADA_NOCHE:
        {
            showActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_PERSONA_DETECTADA_NOCHE");
            turnOnEntranceLight();
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
            showActualState("ESTADO_VALIDACION_CLAVE", "EVENTO_TIMEOUT_VALIDACION_CLAVE");
            // there may be an issue reading NFC card
            lcd.showMessageFullScreen("Error validando clave");
            state = ESTADO_ESPERANDO_INGRESO_CONTRASENA;
        }
        break;

        case EVENTO_CLAVE_VALIDA:
        {
            showActualState("ESTADO_VALIDACION_CLAVE", "EVENTO_CLAVE_VALIDA");
            reproduceValidPassSoundInBuzzer();
            unlockEntranceDoor();
            showValidPassMessageOnScreen();
            clearPassEnteredIntoLock();
            state = ESTADO_ESPERANDO_APERTURA_PUERTA;
        }
        break;

        case EVENTO_CLAVE_INVALIDA:
        {
            showActualState("ESTADO_VALIDACION_CLAVE", "EVENTO_CLAVE_INVALIDA");
            reproduceInvalidPassSoundInBuzzer();
            clearPassEnteredOnScreen();
            clearPassEnteredIntoLock();
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
        case EVENTO_PERSONA_NO_DETECTADA:
        {
            showActualState("ESTADO_ESPERANDO_APERTURA_PUERTA", "EVENTO_PERSONA_NO_DETECTADA");
            shutdownScreen();
            turnOffEntranceLight();
            clearPassEnteredIntoLock();
            lockEntranceDoor();
            state = ESTADO_BLOQUEADO_ESPERANDO_VISITA;
        }
        break;

        case EVENTO_TIMEOUT_APERTURA_PUERTA:
        {
            showActualState("ESTADO_ESPERANDO_APERTURA_PUERTA", "EVENTO_TIMEOUT_APERTURA_PUERTA");
            lockEntranceDoor();
            initializeScreenToInputPassword();
            reproduceInvalidPassSoundInBuzzer();
            state = ESTADO_ESPERANDO_INGRESO_CONTRASENA;
        }
        break;

        case EVENTO_SE_ABRIO_PUERTA:
        {
            showActualState("ESTADO_ESPERANDO_APERTURA_PUERTA", "EVENTO_SE_ABRIO_PUERTA");
            showOpenDoorMessageOnScreen();
            cancelDoorOpenTimer();
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
    {
        switch (event)
        {
        case EVENTO_SE_CERRO_PUERTA:
        {
            showActualState("ESTADO_ESPERANDO_ENTRADA_PERSONA", "EVENTO_SE_CERRO_PUERTA");
            shutdownScreen();
            turnOffEntranceLight();
            clearPassEnteredIntoLock();
            lockEntranceDoor();
            state = ESTADO_BLOQUEADO_ESPERANDO_VISITA;
        }
        break;

        case EVENTO_NOTIFICAR_PUERTA_ABIERTA:
        {
            showActualState("ESTADO_ESPERANDO_ENTRADA_PERSONA", "EVENTO_NOTIFICAR_PUERTA_ABIERTA");
            state = ESTADO_ESPERANDO_ENTRADA_PERSONA;
        }
        break;

        case EVENTO_CONTINUE:
        {
            state = ESTADO_ESPERANDO_ENTRADA_PERSONA;
        }
        break;
        }
    }
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

void initializeScreenToInputPassword()
{
    lcd.turnOn();
    lcd.loadInputPassScreen();
}

void clearPassEnteredOnScreen()
{
    lcd.resetInputPassScreen();
}

void showPasswordCharPressedOnScreen()
{
    lcd.showKeyPressed(keypad.getLastKeyPressed());
}

void showValidPassMessageOnScreen()
{
    lcd.clear();
    lcd.showMessaggeInLine(0, "Clave correcta");
    lcd.showMessaggeInLine(1, "Empuje la puerta");
}

void shutdownScreen()
{
    lcd.clear();
    lcd.turnOff();
}

void turnOnEntranceLight()
{
    if (light.getIsOn() == false)
    {
        light.turnOn();
    }
}

void turnOffEntranceLight()
{
    if (light.getIsOn() == true)
    {
        light.turnOff();
    }
}

void clearPassEnteredIntoLock()
{
    doorLock.resetPassEntered();
}

void reproduceInvalidPassSoundInBuzzer()
{
    buzzer.activateErrorSound();
}

void reproduceValidPassSoundInBuzzer()
{
    buzzer.activateSuccessSound();
}

void reproduceKeyPressedSoundInBuzzer()
{
    buzzer.activateKeyPressedSound();
}

void startPasswordValidation()
{
    doorLock.changeUnlockInProgress(true);
}

void loadPasswordCharPressedIntoLock()
{
    doorLock.loadCharacter(keypad.getLastKeyPressed());
}

void unlockEntranceDoor()
{
    entranceDoor.unlock();
}

void lockEntranceDoor()
{
    entranceDoor.lock();
}

void showOpenDoorMessageOnScreen()
{
    lcd.showMessageFullScreen("Puerta abierta - Pase");
}

void cancelDoorOpenTimer()
{
    doorLock.changeUnlockInProgress(false);
}