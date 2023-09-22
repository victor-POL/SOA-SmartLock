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
enum State state;
enum Event event;
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

  state = State::CerraduraInit;
  event = Event::Continue;

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

  event = Event::Continue;
}

void stateMachine()
{
  generateEvent();

  switch (state)
  {
  case State::CerraduraInit:
  {
    switch (event)
    {
    case Event::ClaveNoConfigurada:
    {
      showActualState("ESTADO_CERRADURA_INIT", "EVENTO_CLAVE_NO_CONFIGURADA");
      state = State::BloqueadoEsperandoClaveInicial;
    }
    break;
    case Event::ClaveConfigurada:
    {
      showActualState("ESTADO_CERRADURA_INIT", "EVENTO_CLAVE_CONFIGURADA");
      state = State::BloqueadoEsperandoVisita;
    }
    break;
    case Event::Continue:
    {
      showActualState("ESTADO_CERRADURA_INIT", "EVENTO_CONTINUE");
      state = State::CerraduraInit;
    }
    break;
    }
  }
  break;

  case State::BloqueadoEsperandoClaveInicial:
  {
    switch (event)
    {
    case Event::PersonaDetectadaDia:
    {
      showActualState("ESTADO_BLOQUEADO_ESPERANDO_CLAVE_INICIAL", "EVENTO_PERSONA_DETECTADA_DIA");
      initializeScreenToInputNewPassword();
      turnOffEntranceLight();
      state = State::EsperandoIngresoNuevaClave;
    }
    break;

    case Event::PersonaDetectadaNoche:
    {
      showActualState("ESTADO_BLOQUEADO_ESPERANDO_CLAVE_INICIAL", "EVENTO_PERSONA_DETECTADA_NOCHE");
      initializeScreenToInputNewPassword();
      turnOnEntranceLight();
      state = State::EsperandoIngresoNuevaClave;
    }
    break;

    case Event::Continue:
    {
      state = State::BloqueadoEsperandoClaveInicial;
    }
    break;
    }
  }
  break;

  case State::EsperandoIngresoNuevaClave:
  {
    switch (event)
    {
    case Event::PersonaNoDetectada:
    {
      showActualState("ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE", "EVENTO_PERSONA_NO_DETECTADA");
      shutdownScreen();
      turnOffEntranceLight();
      clearNewPassEnteredIntoLock();
      state = State::BloqueadoEsperandoClaveInicial;
    }
    break;

    case Event::ValidarClave:
    {
      showActualState("ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE", "EVENTO_VALIDAR_CLAVE");
      reproduceKeyPressedSoundInBuzzer();
      startNewPasswordConfirmation();
      initializeScreenToInputPasswordConfirmation();
      state = State::ConfirmacionNuevaClave;
    }
    break;

    case Event::CaracterIngresado:
    {
      showActualState("ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE", "EVENTO_CARACTER_INGRESADO");
      reproduceKeyPressedSoundInBuzzer();
      showPasswordCharPressedOnScreen();
      loadNewPasswordCharPressedIntoLock();
      state = State::EsperandoIngresoNuevaClave;
    }
    break;

    case Event::ClearClaveIngresada:
    {
      showActualState("ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE", "EVENTO_CLEAR_CLAVE_INGRESADA");
      reproduceKeyPressedSoundInBuzzer();
      clearPassEnteredOnScreen();
      clearNewPassEnteredIntoLock();
      state = State::EsperandoIngresoNuevaClave;
    }
    break;

    case Event::PersonaDetectadaDia:
    {
      showActualState("ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE", "EVENTO_PERSONA_DETECTADA_DIA");
      turnOffEntranceLight();
      state = State::EsperandoIngresoNuevaClave;
    }
    break;

    case Event::PersonaDetectadaNoche:
    {
      showActualState("ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE", "EVENTO_PERSONA_DETECTADA_NOCHE");
      turnOnEntranceLight();
      state = State::EsperandoIngresoNuevaClave;
    }
    break;

    case Event::Continue:
    {
      state = State::EsperandoIngresoNuevaClave;
    }
    break;
    }
  }
  break;

  case State::ConfirmacionNuevaClave:
  {
    switch (event)
    {
    case Event::PersonaNoDetectada:
    {
      showActualState("ESTADO_CONFIRMACION_NUEVA_CLAVE", "EVENTO_PERSONA_NO_DETECTADA");
      shutdownScreen();
      turnOffEntranceLight();
      clearNewPassEnteredIntoLock();
      state = State::BloqueadoEsperandoClaveInicial;
    }
    break;

    case Event::ValidarClave:
    {
      showActualState("ESTADO_CONFIRMACION_NUEVA_CLAVE", "EVENTO_VALIDAR_CLAVE");
      reproduceKeyPressedSoundInBuzzer();
      startNewPasswordValidation();
      state = State::ValidacionNuevaClave;
    }
    break;

    case Event::CaracterIngresado:
    {
      showActualState("ESTADO_CONFIRMACION_NUEVA_CLAVE", "EVENTO_CARACTER_INGRESADO");
      reproduceKeyPressedSoundInBuzzer();
      showPasswordCharPressedOnScreen();
      loadNewPasswordCharPressedIntoLock();
      state = State::ConfirmacionNuevaClave;
    }
    break;

    case Event::ClearClaveIngresada:
    {
      showActualState("ESTADO_CONFIRMACION_NUEVA_CLAVE", "EVENTO_CLEAR_CLAVE_INGRESADA");
      reproduceKeyPressedSoundInBuzzer();
      clearPassEnteredOnScreen();
      clearNewPassEnteredIntoLock();
      state = State::ConfirmacionNuevaClave;
    }
    break;

    case Event::PersonaDetectadaDia:
    {
      showActualState("ESTADO_CONFIRMACION_NUEVA_CLAVE", "EVENTO_PERSONA_DETECTADA_DIA");
      turnOffEntranceLight();
      state = State::ConfirmacionNuevaClave;
    }
    break;

    case Event::PersonaDetectadaNoche:
    {
      showActualState("ESTADO_CONFIRMACION_NUEVA_CLAVE", "EVENTO_PERSONA_DETECTADA_NOCHE");
      turnOnEntranceLight();
      state = State::ConfirmacionNuevaClave;
    }
    break;

    case Event::Continue:
    {
      state = State::ConfirmacionNuevaClave;
    }
    break;
    }
  }
  break;

  case State::ValidacionNuevaClave:
  {
    switch (event)
    {
    case Event::ClaveValida:
    {
      showActualState("ESTADO_VALIDACION_NUEVA_CLAVE", "EVENTO_CLAVE_VALIDA");
      reproduceValidPassSoundInBuzzer();
      shutdownScreen();
      turnOffEntranceLight();
      state = State::BloqueadoEsperandoVisita;
    }
    break;

    case Event::ClaveInvalida:
    {
      showActualState("ESTADO_VALIDACION_NUEVA_CLAVE", "EVENTO_CLAVE_INVALIDA");
      reproduceInvalidPassSoundInBuzzer();
      initializeScreenToInputNewPassword();
      clearNewPassEnteredIntoLock();
      state = State::EsperandoIngresoNuevaClave;
    }
    break;

    case Event::Continue:
    {
      state = State::ValidacionNuevaClave;
    }
    break;
    }
  }
  break;

  case State::BloqueadoEsperandoVisita:
  {
    switch (event)
    {
    case Event::BotonPresionado:
    {
      showActualState("ESTADO_BLOQUEADO_ESPERANDO_VISITA", "EVENTO_BOTON_PRESIONADO");
      doorLock.UnlockWithButton();
      unlockEntranceDoor();
      state = State::EsperandoAperturaPuertaBoton;
    }
    break;

    case Event::PersonaDetectadaDia:
    {
      showActualState("ESTADO_BLOQUEADO_ESPERANDO_VISITA", "EVENTO_PERSONA_DETECTADA_DIA");
      initializeScreenToInputPassword();
      turnOffEntranceLight();
      state = State::EsperandoIngresoClave;
    }
    break;

    case Event::PersonaDetectadaNoche:
    {
      showActualState("ESTADO_BLOQUEADO_ESPERANDO_VISITA", "EVENTO_PERSONA_DETECTADA_NOCHE");
      initializeScreenToInputPassword();
      turnOnEntranceLight();
      state = State::EsperandoIngresoClave;
    }
    break;

    case Event::PersonaNoDetectada:
    {
      showActualState("ESTADO_BLOQUEADO_ESPERANDO_VISITA", "EVENTO_PERSONA_NO_DETECTADA");
      state = State::BloqueadoEsperandoVisita;
    }
    break;

    case Event::Continue:
    {
      state = State::BloqueadoEsperandoVisita;
    }
    break;
    }
  }
  break;

  case State::EsperandoIngresoClave:
  {
    switch (event)
    {
    case Event::PersonaNoDetectada:
    {
      showActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_PERSONA_NO_DETECTADA");
      shutdownScreen();
      turnOffEntranceLight();
      clearPassEnteredIntoLock();
      state = State::BloqueadoEsperandoVisita;
    }
    break;

    case Event::ValidarClave:
    {
      showActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_VALIDAR_CLAVE");
      reproduceKeyPressedSoundInBuzzer();
      startPasswordValidation();
      state = State::ValidacionClave;
    }
    break;

    case Event::CaracterIngresado:
    {
      showActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_CARACTER_INGRESADO");
      reproduceKeyPressedSoundInBuzzer();
      showPasswordCharPressedOnScreen();
      loadPasswordCharPressedIntoLock();
      state = State::EsperandoIngresoClave;
    }
    break;

    case Event::ClearClaveIngresada:
    {
      showActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_CLEAR_CLAVE_INGRESADA");
      reproduceKeyPressedSoundInBuzzer();
      clearPassEnteredOnScreen();
      clearPassEnteredIntoLock();
      state = State::EsperandoIngresoClave;
    }
    break;

    case Event::PersonaDetectadaDia:
    {
      showActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_PERSONA_DETECTADA_DIA");
      turnOffEntranceLight();
      state = State::EsperandoIngresoClave;
    }
    break;

    case Event::PersonaDetectadaNoche:
    {
      showActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_PERSONA_DETECTADA_NOCHE");
      turnOnEntranceLight();
      state = State::EsperandoIngresoClave;
    }
    break;

    case Event::Continue:
    {
      state = State::EsperandoIngresoClave;
    }
    break;
    }
  }
  break;

  case State::ValidacionClave:
  {
    switch (event)
    {
    case Event::TimeOutValidacionClave:
    {
      showActualState("ESTADO_VALIDACION_CLAVE", "EVENTO_TIMEOUT_VALIDACION_CLAVE");
      state = State::EsperandoIngresoClave;
    }
    break;

    case Event::ClaveValida:
    {
      showActualState("ESTADO_VALIDACION_CLAVE", "EVENTO_CLAVE_VALIDA");
      reproduceValidPassSoundInBuzzer();
      unlockEntranceDoor();
      showValidPassMessageOnScreen();
      clearPassEnteredIntoLock();
      state = State::EsperandoAperturaPuerta;
    }
    break;

    case Event::ClaveInvalida:
    {
      showActualState("ESTADO_VALIDACION_CLAVE", "EVENTO_CLAVE_INVALIDA");
      reproduceInvalidPassSoundInBuzzer();
      clearPassEnteredOnScreen();
      clearPassEnteredIntoLock();
      state = State::EsperandoIngresoClave;
    }
    break;

    case Event::Continue:
    {
      state = State::ValidacionClave;
    }
    break;
    }
  }
  break;

  case State::EsperandoAperturaPuerta:
  {
    switch (event)
    {
    case Event::PersonaNoDetectada:
    {
      showActualState("ESTADO_ESPERANDO_APERTURA_PUERTA", "EVENTO_PERSONA_NO_DETECTADA");
      shutdownScreen();
      turnOffEntranceLight();
      clearPassEnteredIntoLock();
      lockEntranceDoor();
      state = State::BloqueadoEsperandoVisita;
    }
    break;

    case Event::TimeOutAperturaPuerta:
    {
      showActualState("ESTADO_ESPERANDO_APERTURA_PUERTA", "EVENTO_TIMEOUT_APERTURA_PUERTA");
      shutdownScreen();
      turnOffEntranceLight();
      clearPassEnteredIntoLock();
      lockEntranceDoor();
      reproduceInvalidPassSoundInBuzzer();
      state = State::BloqueadoEsperandoVisita;
    }
    break;

    case Event::SeAbrioPuerta:
    {
      showActualState("ESTADO_ESPERANDO_APERTURA_PUERTA", "EVENTO_SE_ABRIO_PUERTA");
      showOpenDoorMessageOnScreen();
      cancelDoorOpenTimer();
      state = State::EsperandoEntradaPersona;
    }
    break;

    case Event::Continue:
    {
      state = State::EsperandoAperturaPuerta;
    }
    break;
    }
  }
  break;

  case State::EsperandoAperturaPuertaBoton:
  {
    switch (event)
    {
    case Event::SeAbrioPuerta:
    {
      showActualState("ESTADO_ESPERANDO_APERTURA_PUERTA_BOTON", "EVENTO_SE_ABRIO_PUERTA");
      state = State::EsperandoEntradaPersona;
    }
    break;

    case Event::TimeOutAperturaPuerta:
    {
      showActualState("EVENTO_TIMEOUT_APERTURA_PUERTA", "ESTADO_BLOQUEADO_ESPERANDO_VISITA");
      lockEntranceDoor();
      state = State::BloqueadoEsperandoVisita;
    }
    break;

    case Event::Continue:
    {
      state = State::EsperandoAperturaPuerta;
    }
    break;
    }
  }
  break;

  case State::EsperandoEntradaPersona:
  {
    switch (event)
    {
    case Event::SeCerroPuerta:
    {
      showActualState("ESTADO_ESPERANDO_ENTRADA_PERSONA", "EVENTO_SE_CERRO_PUERTA");
      shutdownScreen();
      turnOffEntranceLight();
      clearPassEnteredIntoLock();
      lockEntranceDoor();
      state = State::BloqueadoEsperandoVisita;
    }
    break;

    case Event::NotificarPuertaAbierta:
    {
      showActualState("ESTADO_ESPERANDO_ENTRADA_PERSONA", "EVENTO_NOTIFICAR_PUERTA_ABIERTA");
      state = State::EsperandoEntradaPersona;
    }
    break;

    case Event::Continue:
    {
      state = State::EsperandoEntradaPersona;
    }
    break;
    }
  }
  break;
  }

  event = Event::Continue;
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