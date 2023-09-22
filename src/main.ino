#include "main.h"

// Sensors
UltrasonicEntrance entranceSensor = UltrasonicEntrance(ENTRANCE_SENSOR_TRIGGER_PIN, ENTRANCE_SENSOR_ECHO_PIN);
UltrasonicDoor doorSensor = UltrasonicDoor(DOOR_SENSOR_TRIGGER_PIN, DOOR_SENSOR_ECHO_PIN);
KeyPad keypad = KeyPad();
Button button = Button();

// Actuators
MyServo entranceDoor = MyServo(SERVO_PIN);
Buzzer buzzer = Buzzer(BUZZER_PIN);
Relay light = Relay(RELAY_PIN);
LCD *LCD::instance = NULL;
LCD lcd = *LCD::GetInstance();

// Lock
Locker doorLock = Locker();

// Global
enum States state;
enum Events event;
bool timeout;
long lastCurrentTime;

void doInit()
{
  Serial.begin(115200);

  entranceDoor.Setup();
  entranceSensor.Setup();
  doorSensor.Setup();
  button.Setup();

  buzzer.Setup();
  light.Setup();
  lcd.Setup();

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

    buzzer.CheckStatus();

    if (button.CheckStatus() || doorLock.CheckStatus() || keypad.CheckStatus() || doorSensor.CheckStatus() || entranceSensor.CheckStatus())
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
      state = ESTADO_BLOQUEADO_ESPERANDO_CLAVE_INICIAL;
    }
    break;
    case EVENTO_CLAVE_CONFIGURADA:
    {
      showActualState("ESTADO_CERRADURA_INIT", "EVENTO_CLAVE_CONFIGURADA");
      state = ESTADO_BLOQUEADO_ESPERANDO_VISITA;
    }
    break;
    case EVENTO_CONTINUE:
    {
      showActualState("ESTADO_CERRADURA_INIT", "EVENTO_CONTINUE");
      state = ESTADO_CERRADURA_INIT;
    }
    break;
    }
  }
  break;

  case ESTADO_BLOQUEADO_ESPERANDO_CLAVE_INICIAL:
  {
    switch (event)
    {
    case EVENTO_PERSONA_DETECTADA_DIA:
    {
      showActualState("ESTADO_BLOQUEADO_ESPERANDO_CLAVE_INICIAL", "EVENTO_PERSONA_DETECTADA_DIA");
      initializeScreenToInputNewPassword();
      turnOffEntranceLight();
      state = ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE;
    }
    break;

    case EVENTO_PERSONA_DETECTADA_NOCHE:
    {
      showActualState("ESTADO_BLOQUEADO_ESPERANDO_CLAVE_INICIAL", "EVENTO_PERSONA_DETECTADA_NOCHE");
      initializeScreenToInputNewPassword();
      turnOnEntranceLight();
      state = ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE;
    }
    break;

    case EVENTO_CONTINUE:
    {
      state = ESTADO_BLOQUEADO_ESPERANDO_CLAVE_INICIAL;
    }
    break;
    }
  }
  break;

  case ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE:
  {
    switch (event)
    {
    case EVENTO_PERSONA_NO_DETECTADA:
    {
      showActualState("ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE", "EVENTO_PERSONA_NO_DETECTADA");
      shutdownScreen();
      turnOffEntranceLight();
      clearNewPassEnteredIntoLock();
      state = ESTADO_BLOQUEADO_ESPERANDO_CLAVE_INICIAL;
    }
    break;

    case EVENTO_VALIDAR_CLAVE:
    {
      showActualState("ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE", "EVENTO_VALIDAR_CLAVE");
      reproduceKeyPressedSoundInBuzzer();
      startNewPasswordConfirmation();
      initializeScreenToInputPasswordConfirmation();
      state = ESTADO_CONFIRMACION_NUEVA_CLAVE;
    }
    break;

    case EVENTO_CARACTER_INGRESADO:
    {
      showActualState("ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE", "EVENTO_CARACTER_INGRESADO");
      reproduceKeyPressedSoundInBuzzer();
      showPasswordCharPressedOnScreen();
      loadNewPasswordCharPressedIntoLock();
      state = ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE;
    }
    break;

    case EVENTO_CLEAR_CLAVE_INGRESADA:
    {
      showActualState("ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE", "EVENTO_CLEAR_CLAVE_INGRESADA");
      reproduceKeyPressedSoundInBuzzer();
      clearPassEnteredOnScreen();
      clearNewPassEnteredIntoLock();
      state = ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE;
    }
    break;

    case EVENTO_PERSONA_DETECTADA_DIA:
    {
      showActualState("ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE", "EVENTO_PERSONA_DETECTADA_DIA");
      turnOffEntranceLight();
      state = ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE;
    }
    break;

    case EVENTO_PERSONA_DETECTADA_NOCHE:
    {
      showActualState("ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE", "EVENTO_PERSONA_DETECTADA_NOCHE");
      turnOnEntranceLight();
      state = ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE;
    }
    break;

    case EVENTO_CONTINUE:
    {
      state = ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE;
    }
    break;
    }
  }
  break;

  case ESTADO_CONFIRMACION_NUEVA_CLAVE:
  {
    switch (event)
    {
    case EVENTO_PERSONA_NO_DETECTADA:
    {
      showActualState("ESTADO_CONFIRMACION_NUEVA_CLAVE", "EVENTO_PERSONA_NO_DETECTADA");
      shutdownScreen();
      turnOffEntranceLight();
      clearNewPassEnteredIntoLock();
      state = ESTADO_BLOQUEADO_ESPERANDO_CLAVE_INICIAL;
    }
    break;

    case EVENTO_VALIDAR_CLAVE:
    {
      showActualState("ESTADO_CONFIRMACION_NUEVA_CLAVE", "EVENTO_VALIDAR_CLAVE");
      reproduceKeyPressedSoundInBuzzer();
      startNewPasswordValidation();
      state = ESTADO_VALIDACION_NUEVA_CLAVE;
    }
    break;

    case EVENTO_CARACTER_INGRESADO:
    {
      showActualState("ESTADO_CONFIRMACION_NUEVA_CLAVE", "EVENTO_CARACTER_INGRESADO");
      reproduceKeyPressedSoundInBuzzer();
      showPasswordCharPressedOnScreen();
      loadNewPasswordCharPressedIntoLock();
      state = ESTADO_CONFIRMACION_NUEVA_CLAVE;
    }
    break;

    case EVENTO_CLEAR_CLAVE_INGRESADA:
    {
      showActualState("ESTADO_CONFIRMACION_NUEVA_CLAVE", "EVENTO_CLEAR_CLAVE_INGRESADA");
      reproduceKeyPressedSoundInBuzzer();
      clearPassEnteredOnScreen();
      clearNewPassEnteredIntoLock();
      state = ESTADO_CONFIRMACION_NUEVA_CLAVE;
    }
    break;

    case EVENTO_PERSONA_DETECTADA_DIA:
    {
      showActualState("ESTADO_CONFIRMACION_NUEVA_CLAVE", "EVENTO_PERSONA_DETECTADA_DIA");
      turnOffEntranceLight();
      state = ESTADO_CONFIRMACION_NUEVA_CLAVE;
    }
    break;

    case EVENTO_PERSONA_DETECTADA_NOCHE:
    {
      showActualState("ESTADO_CONFIRMACION_NUEVA_CLAVE", "EVENTO_PERSONA_DETECTADA_NOCHE");
      turnOnEntranceLight();
      state = ESTADO_CONFIRMACION_NUEVA_CLAVE;
    }
    break;

    case EVENTO_CONTINUE:
    {
      state = ESTADO_CONFIRMACION_NUEVA_CLAVE;
    }
    break;
    }
  }
  break;

  case ESTADO_VALIDACION_NUEVA_CLAVE:
  {
    switch (event)
    {
    case EVENTO_CLAVE_VALIDA:
    {
      showActualState("ESTADO_VALIDACION_NUEVA_CLAVE", "EVENTO_CLAVE_VALIDA");
      reproduceValidPassSoundInBuzzer();
      shutdownScreen();
      turnOffEntranceLight();
      state = ESTADO_BLOQUEADO_ESPERANDO_VISITA;
    }
    break;

    case EVENTO_CLAVE_INVALIDA:
    {
      showActualState("ESTADO_VALIDACION_NUEVA_CLAVE", "EVENTO_CLAVE_INVALIDA");
      reproduceInvalidPassSoundInBuzzer();
      initializeScreenToInputNewPassword();
      clearNewPassEnteredIntoLock();
      state = ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE;
    }
    break;

    case EVENTO_CONTINUE:
    {
      state = ESTADO_VALIDACION_NUEVA_CLAVE;
    }
    break;
    }
  }
  break;

  case ESTADO_BLOQUEADO_ESPERANDO_VISITA:
  {
    switch (event)
    {
    case EVENTO_BOTON_PRESIONADO:
    {
      showActualState("ESTADO_BLOQUEADO_ESPERANDO_VISITA", "EVENTO_BOTON_PRESIONADO");
      doorLock.UnlockWithButton();
      unlockEntranceDoor();
      state = ESTADO_ESPERANDO_APERTURA_PUERTA_BOTON;
    }
    break;

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
      shutdownScreen();
      turnOffEntranceLight();
      clearPassEnteredIntoLock();
      lockEntranceDoor();
      reproduceInvalidPassSoundInBuzzer();
      state = ESTADO_BLOQUEADO_ESPERANDO_VISITA;
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

  case ESTADO_ESPERANDO_APERTURA_PUERTA_BOTON:
  {
    switch (event)
    {
    case EVENTO_SE_ABRIO_PUERTA:
    {
      showActualState("ESTADO_ESPERANDO_APERTURA_PUERTA_BOTON", "EVENTO_SE_ABRIO_PUERTA");
      state = ESTADO_ESPERANDO_ENTRADA_PERSONA;
    }
    break;

    case EVENTO_TIMEOUT_APERTURA_PUERTA:
    {
      showActualState("EVENTO_TIMEOUT_APERTURA_PUERTA", "ESTADO_BLOQUEADO_ESPERANDO_VISITA");
      lockEntranceDoor();
      state = ESTADO_BLOQUEADO_ESPERANDO_VISITA;
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
  lcd.TurnOn();
  lcd.LoadInputPassScreen();
}

void initializeScreenToInputNewPassword()
{
  lcd.TurnOn();
  lcd.LoadNewPassScreen();
}

void initializeScreenToInputPasswordConfirmation()
{
  lcd.TurnOn();
  lcd.LoadConfirmNewPassScreen();
}

void clearPassEnteredOnScreen()
{
  lcd.ResetInputPassScreen();
}

void showPasswordCharPressedOnScreen()
{
  lcd.ShowKeyPressed(keypad.get_last_key_pressed());
}

void showValidPassMessageOnScreen()
{
  lcd.ShowMessage("Clave correcta", "Empuje la puerta");
}

void shutdownScreen()
{
  lcd.Clear();
  lcd.TurnOff();
}

void turnOnEntranceLight()
{
  if (light.get_is_on() == false)
  {
    light.TurnOn();
  }
}

void turnOffEntranceLight()
{
  if (light.get_is_on() == true)
  {
    light.TurnOff();
  }
}

void clearPassEnteredIntoLock()
{
  doorLock.ResetPassEntered();
}

void clearNewPassEnteredIntoLock()
{
  doorLock.ResetNewPassEntered();
}

void reproduceInvalidPassSoundInBuzzer()
{
  buzzer.ActivateErrorSound();
}

void reproduceValidPassSoundInBuzzer()
{
  buzzer.ActivateSuccessSound();
}

void reproduceKeyPressedSoundInBuzzer()
{
  buzzer.ActivateKeyPressedSound();
}

void startPasswordValidation()
{
  doorLock.ChangeUnlockInProgress(true);
}

void startNewPasswordConfirmation()
{
  doorLock.ToPasswordConfirmation();
}

void startNewPasswordValidation()
{
  doorLock.ChangeNewPassInProgress(true);
}

void loadPasswordCharPressedIntoLock()
{
  doorLock.LoadCharacter(keypad.get_last_key_pressed());
}

void loadNewPasswordCharPressedIntoLock()
{
  doorLock.LoadNewCharacter(keypad.get_last_key_pressed());
}

void unlockEntranceDoor()
{
  entranceDoor.Unlock();
}

void lockEntranceDoor()
{
  entranceDoor.Lock();
}

void showOpenDoorMessageOnScreen()
{
  lcd.TurnOn();
  lcd.ShowMessage("Puerta abierta", "pase");
}

void cancelDoorOpenTimer()
{
  doorLock.ChangeUnlockInProgress(false);
}