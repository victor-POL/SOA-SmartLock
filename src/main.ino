#include "main.h"

// Sensors
UltrasonicEntrance entrance_sensor = UltrasonicEntrance(ENTRANCE_SENSOR_TRIGGER_PIN, ENTRANCE_SENSOR_ECHO_PIN);
UltrasonicDoor door_sensor = UltrasonicDoor(DOOR_SENSOR_TRIGGER_PIN, DOOR_SENSOR_ECHO_PIN);
KeyPad keypad = KeyPad();
Button button = Button();

// Actuators
MyServo entrance_door = MyServo(SERVO_PIN);
Buzzer buzzer = Buzzer(BUZZER_PIN);
Relay light = Relay(RELAY_PIN);
LCD *LCD::instance = NULL;
LCD lcd = *LCD::GetInstance();

// Lock
Lock door_lock = Lock();

// Global
enum State state;
enum Event event;
bool timeout;
long last_current_time;

void DoInit()
{
  Serial.begin(115200);

  entrance_door.Setup();
  entrance_sensor.Setup();
  door_sensor.Setup();
  button.Setup();

  buzzer.Setup();
  light.Setup();
  lcd.Setup();

  state = State::CerraduraInit;
  event = Event::Continue;

  timeout = false;
  last_current_time = millis();
}

void GenerateEvent()
{
  long current_time = millis();
  int diff = current_time - last_current_time;
  timeout = (diff > UMBRAL_DIFERENCIA_TIMEOUT) ? true : false;

  if (timeout)
  {
    timeout = false;
    last_current_time = current_time;

    buzzer.CheckStatus();

    if (button.CheckStatus() || door_lock.CheckStatus() || keypad.CheckStatus() || door_sensor.CheckStatus() || entrance_sensor.CheckStatus())
    {
      return;
    }
  }

  event = Event::Continue;
}

void StateMachine()
{
  GenerateEvent();

  switch (state)
  {
  case State::CerraduraInit:
  {
    switch (event)
    {
    case Event::ClaveNoConfigurada:
    {
      ShowActualState("ESTADO_CERRADURA_INIT", "EVENTO_CLAVE_NO_CONFIGURADA");
      state = State::BloqueadoEsperandoClaveInicial;
    }
    break;
    case Event::ClaveConfigurada:
    {
      ShowActualState("ESTADO_CERRADURA_INIT", "EVENTO_CLAVE_CONFIGURADA");
      state = State::BloqueadoEsperandoVisita;
    }
    break;
    case Event::Continue:
    {
      ShowActualState("ESTADO_CERRADURA_INIT", "EVENTO_CONTINUE");
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
      ShowActualState("ESTADO_BLOQUEADO_ESPERANDO_CLAVE_INICIAL", "EVENTO_PERSONA_DETECTADA_DIA");
      InitializeScreenToInputNewPassword();
      TurnOffEntranceLight();
      state = State::EsperandoIngresoNuevaClave;
    }
    break;

    case Event::PersonaDetectadaNoche:
    {
      ShowActualState("ESTADO_BLOQUEADO_ESPERANDO_CLAVE_INICIAL", "EVENTO_PERSONA_DETECTADA_NOCHE");
      InitializeScreenToInputNewPassword();
      TurnOnEntranceLight();
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
      ShowActualState("ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE", "EVENTO_PERSONA_NO_DETECTADA");
      ShutdownScreen();
      TurnOffEntranceLight();
      ClearNewPassEnteredIntoLock();
      state = State::BloqueadoEsperandoClaveInicial;
    }
    break;

    case Event::ValidarClave:
    {
      ShowActualState("ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE", "EVENTO_VALIDAR_CLAVE");
      ReproduceKeyPressedSoundInBuzzer();
      StartNewPasswordConfirmation();
      InitializeScreenToInputPasswordConfirmation();
      state = State::ConfirmacionNuevaClave;
    }
    break;

    case Event::CaracterIngresado:
    {
      ShowActualState("ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE", "EVENTO_CARACTER_INGRESADO");
      ReproduceKeyPressedSoundInBuzzer();
      ShowPasswordCharPressedOnScreen();
      LoadNewPasswordCharPressedIntoLock();
      state = State::EsperandoIngresoNuevaClave;
    }
    break;

    case Event::ClearClaveIngresada:
    {
      ShowActualState("ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE", "EVENTO_CLEAR_CLAVE_INGRESADA");
      ReproduceKeyPressedSoundInBuzzer();
      ClearPassEnteredOnScreen();
      ClearNewPassEnteredIntoLock();
      state = State::EsperandoIngresoNuevaClave;
    }
    break;

    case Event::PersonaDetectadaDia:
    {
      ShowActualState("ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE", "EVENTO_PERSONA_DETECTADA_DIA");
      TurnOffEntranceLight();
      state = State::EsperandoIngresoNuevaClave;
    }
    break;

    case Event::PersonaDetectadaNoche:
    {
      ShowActualState("ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE", "EVENTO_PERSONA_DETECTADA_NOCHE");
      TurnOnEntranceLight();
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
      ShowActualState("ESTADO_CONFIRMACION_NUEVA_CLAVE", "EVENTO_PERSONA_NO_DETECTADA");
      ShutdownScreen();
      TurnOffEntranceLight();
      ClearNewPassEnteredIntoLock();
      state = State::BloqueadoEsperandoClaveInicial;
    }
    break;

    case Event::ValidarClave:
    {
      ShowActualState("ESTADO_CONFIRMACION_NUEVA_CLAVE", "EVENTO_VALIDAR_CLAVE");
      ReproduceKeyPressedSoundInBuzzer();
      StartNewPasswordValidation();
      state = State::ValidacionNuevaClave;
    }
    break;

    case Event::CaracterIngresado:
    {
      ShowActualState("ESTADO_CONFIRMACION_NUEVA_CLAVE", "EVENTO_CARACTER_INGRESADO");
      ReproduceKeyPressedSoundInBuzzer();
      ShowPasswordCharPressedOnScreen();
      LoadNewPasswordCharPressedIntoLock();
      state = State::ConfirmacionNuevaClave;
    }
    break;

    case Event::ClearClaveIngresada:
    {
      ShowActualState("ESTADO_CONFIRMACION_NUEVA_CLAVE", "EVENTO_CLEAR_CLAVE_INGRESADA");
      ReproduceKeyPressedSoundInBuzzer();
      ClearPassEnteredOnScreen();
      ClearNewPassEnteredIntoLock();
      state = State::ConfirmacionNuevaClave;
    }
    break;

    case Event::PersonaDetectadaDia:
    {
      ShowActualState("ESTADO_CONFIRMACION_NUEVA_CLAVE", "EVENTO_PERSONA_DETECTADA_DIA");
      TurnOffEntranceLight();
      state = State::ConfirmacionNuevaClave;
    }
    break;

    case Event::PersonaDetectadaNoche:
    {
      ShowActualState("ESTADO_CONFIRMACION_NUEVA_CLAVE", "EVENTO_PERSONA_DETECTADA_NOCHE");
      TurnOnEntranceLight();
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
      ShowActualState("ESTADO_VALIDACION_NUEVA_CLAVE", "EVENTO_CLAVE_VALIDA");
      ReproduceValidPassSoundInBuzzer();
      ShutdownScreen();
      TurnOffEntranceLight();
      state = State::BloqueadoEsperandoVisita;
    }
    break;

    case Event::ClaveInvalida:
    {
      ShowActualState("ESTADO_VALIDACION_NUEVA_CLAVE", "EVENTO_CLAVE_INVALIDA");
      ReproduceInvalidPassSoundInBuzzer();
      InitializeScreenToInputNewPassword();
      ClearNewPassEnteredIntoLock();
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
      ShowActualState("ESTADO_BLOQUEADO_ESPERANDO_VISITA", "EVENTO_BOTON_PRESIONADO");
      door_lock.UnlockWithButton();
      UnlockEntranceDoor();
      state = State::EsperandoAperturaPuertaBoton;
    }
    break;

    case Event::PersonaDetectadaDia:
    {
      ShowActualState("ESTADO_BLOQUEADO_ESPERANDO_VISITA", "EVENTO_PERSONA_DETECTADA_DIA");
      InitializeScreenToInputPassword();
      TurnOffEntranceLight();
      state = State::EsperandoIngresoClave;
    }
    break;

    case Event::PersonaDetectadaNoche:
    {
      ShowActualState("ESTADO_BLOQUEADO_ESPERANDO_VISITA", "EVENTO_PERSONA_DETECTADA_NOCHE");
      InitializeScreenToInputPassword();
      TurnOnEntranceLight();
      state = State::EsperandoIngresoClave;
    }
    break;

    case Event::PersonaNoDetectada:
    {
      ShowActualState("ESTADO_BLOQUEADO_ESPERANDO_VISITA", "EVENTO_PERSONA_NO_DETECTADA");
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
      ShowActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_PERSONA_NO_DETECTADA");
      ShutdownScreen();
      TurnOffEntranceLight();
      ClearPassEnteredIntoLock();
      state = State::BloqueadoEsperandoVisita;
    }
    break;

    case Event::ValidarClave:
    {
      ShowActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_VALIDAR_CLAVE");
      ReproduceKeyPressedSoundInBuzzer();
      StartPasswordValidation();
      state = State::ValidacionClave;
    }
    break;

    case Event::CaracterIngresado:
    {
      ShowActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_CARACTER_INGRESADO");
      ReproduceKeyPressedSoundInBuzzer();
      ShowPasswordCharPressedOnScreen();
      LoadPasswordCharPressedIntoLock();
      state = State::EsperandoIngresoClave;
    }
    break;

    case Event::ClearClaveIngresada:
    {
      ShowActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_CLEAR_CLAVE_INGRESADA");
      ReproduceKeyPressedSoundInBuzzer();
      ClearPassEnteredOnScreen();
      ClearPassEnteredIntoLock();
      state = State::EsperandoIngresoClave;
    }
    break;

    case Event::PersonaDetectadaDia:
    {
      ShowActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_PERSONA_DETECTADA_DIA");
      TurnOffEntranceLight();
      state = State::EsperandoIngresoClave;
    }
    break;

    case Event::PersonaDetectadaNoche:
    {
      ShowActualState("ESTADO_ESPERANDO_INGRESO_CONTRASENA", "EVENTO_PERSONA_DETECTADA_NOCHE");
      TurnOnEntranceLight();
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
      ShowActualState("ESTADO_VALIDACION_CLAVE", "EVENTO_TIMEOUT_VALIDACION_CLAVE");
      state = State::EsperandoIngresoClave;
    }
    break;

    case Event::ClaveValida:
    {
      ShowActualState("ESTADO_VALIDACION_CLAVE", "EVENTO_CLAVE_VALIDA");
      ReproduceValidPassSoundInBuzzer();
      UnlockEntranceDoor();
      ShowValidPassMessageOnScreen();
      ClearPassEnteredIntoLock();
      state = State::EsperandoAperturaPuerta;
    }
    break;

    case Event::ClaveInvalida:
    {
      ShowActualState("ESTADO_VALIDACION_CLAVE", "EVENTO_CLAVE_INVALIDA");
      ReproduceInvalidPassSoundInBuzzer();
      ClearPassEnteredOnScreen();
      ClearPassEnteredIntoLock();
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
      ShowActualState("ESTADO_ESPERANDO_APERTURA_PUERTA", "EVENTO_PERSONA_NO_DETECTADA");
      ShutdownScreen();
      TurnOffEntranceLight();
      ClearPassEnteredIntoLock();
      LockEntranceDoor();
      state = State::BloqueadoEsperandoVisita;
    }
    break;

    case Event::TimeOutAperturaPuerta:
    {
      ShowActualState("ESTADO_ESPERANDO_APERTURA_PUERTA", "EVENTO_TIMEOUT_APERTURA_PUERTA");
      ShutdownScreen();
      TurnOffEntranceLight();
      ClearPassEnteredIntoLock();
      LockEntranceDoor();
      ReproduceInvalidPassSoundInBuzzer();
      state = State::BloqueadoEsperandoVisita;
    }
    break;

    case Event::SeAbrioPuerta:
    {
      ShowActualState("ESTADO_ESPERANDO_APERTURA_PUERTA", "EVENTO_SE_ABRIO_PUERTA");
      ShowOpenDoorMessageOnScreen();
      CancelDoorOpenTimer();
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
      ShowActualState("ESTADO_ESPERANDO_APERTURA_PUERTA_BOTON", "EVENTO_SE_ABRIO_PUERTA");
      state = State::EsperandoEntradaPersona;
    }
    break;

    case Event::TimeOutAperturaPuerta:
    {
      ShowActualState("EVENTO_TIMEOUT_APERTURA_PUERTA", "ESTADO_BLOQUEADO_ESPERANDO_VISITA");
      LockEntranceDoor();
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
      ShowActualState("ESTADO_ESPERANDO_ENTRADA_PERSONA", "EVENTO_SE_CERRO_PUERTA");
      ShutdownScreen();
      TurnOffEntranceLight();
      ClearPassEnteredIntoLock();
      LockEntranceDoor();
      state = State::BloqueadoEsperandoVisita;
    }
    break;

    case Event::NotificarPuertaAbierta:
    {
      ShowActualState("ESTADO_ESPERANDO_ENTRADA_PERSONA", "EVENTO_NOTIFICAR_PUERTA_ABIERTA");
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
  DoInit();
}

void loop()
{
  StateMachine();
}

void ShowActualState(String strState, String strEvent)
{
  Serial.println("-----------------------------------------------------");
  Serial.println("Estado: " + String(strState));
  Serial.println("Evento: " + String(strEvent));
  Serial.println("-----------------------------------------------------");
}

void InitializeScreenToInputPassword()
{
  lcd.TurnOn();
  lcd.LoadInputPassScreen();
}

void InitializeScreenToInputNewPassword()
{
  lcd.TurnOn();
  lcd.LoadNewPassScreen();
}

void InitializeScreenToInputPasswordConfirmation()
{
  lcd.TurnOn();
  lcd.LoadConfirmNewPassScreen();
}

void ClearPassEnteredOnScreen()
{
  lcd.ResetInputPassScreen();
}

void ShowPasswordCharPressedOnScreen()
{
  lcd.ShowKeyPressed(keypad.get_last_key_pressed());
}

void ShowValidPassMessageOnScreen()
{
  lcd.ShowMessage("Clave correcta", "Empuje la puerta");
}

void ShutdownScreen()
{
  lcd.Clear();
  lcd.TurnOff();
}

void TurnOnEntranceLight()
{
  if (light.get_is_on() == false)
  {
    light.TurnOn();
  }
}

void TurnOffEntranceLight()
{
  if (light.get_is_on() == true)
  {
    light.TurnOff();
  }
}

void ClearPassEnteredIntoLock()
{
  door_lock.ResetPassEntered();
}

void ClearNewPassEnteredIntoLock()
{
  door_lock.ResetNewPassEntered();
}

void ReproduceInvalidPassSoundInBuzzer()
{
  buzzer.ActivateErrorSound();
}

void ReproduceValidPassSoundInBuzzer()
{
  buzzer.ActivateSuccessSound();
}

void ReproduceKeyPressedSoundInBuzzer()
{
  buzzer.ActivateKeyPressedSound();
}

void StartPasswordValidation()
{
  door_lock.ChangeUnlockInProgress(true);
}

void StartNewPasswordConfirmation()
{
  door_lock.ToPasswordConfirmation();
}

void StartNewPasswordValidation()
{
  door_lock.ChangeNewPassInProgress(true);
}

void LoadPasswordCharPressedIntoLock()
{
  door_lock.LoadCharacter(keypad.get_last_key_pressed());
}

void LoadNewPasswordCharPressedIntoLock()
{
  door_lock.LoadNewCharacter(keypad.get_last_key_pressed());
}

void UnlockEntranceDoor()
{
  entrance_door.Unlock();
}

void LockEntranceDoor()
{
  entrance_door.Lock();
}

void ShowOpenDoorMessageOnScreen()
{
  lcd.TurnOn();
  lcd.ShowMessage("Puerta abierta", "pase");
}

void CancelDoorOpenTimer()
{
  door_lock.ChangeUnlockInProgress(false);
}