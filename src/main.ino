#include "main.h"

// Sensors
UltrasonicEntrance entrance_sensor = UltrasonicEntrance(ENTRANCE_SENSOR_TRIGGER_PIN, ENTRANCE_SENSOR_ECHO_PIN);
UltrasonicDoor door_sensor = UltrasonicDoor(DOOR_SENSOR_TRIGGER_PIN, DOOR_SENSOR_ECHO_PIN);
KeyPad keypad = KeyPad();
Button button = Button(BUTTON_PIN);

// Actuators
MyServo entrance_door = MyServo(SERVO_PIN);
Buzzer buzzer = Buzzer(BUZZER_PIN);
Relay light = Relay(RELAY_PIN);
#if COMPILAR_PARA_SIMULADOR
LCD *LCD::instance = NULL;
LCD lcd = *LCD::GetInstance();
#else
LCDRgb *LCDRgb::instance = NULL;
LCDRgb lcd = *LCDRgb::GetInstance();
#endif


// Lock
Lock door_lock = Lock();

// Global
enum State state;
enum Event event;
bool timeout;
long last_current_time;

// Funciones para eventos
void None(){}

void CreatePass()
{
  state = State::BloqueadoEsperandoClaveInicial;
}

void PassSetted()
{
  state = State::BloqueadoEsperandoVisita;
}

void NewPassWithLight()
{
  InitializeScreenToInputNewPassword();
  TurnOffEntranceLight();
  state = State::EsperandoIngresoNuevaClave;
}

void NewPassWithoutLight()
{
  InitializeScreenToInputNewPassword();
  TurnOnEntranceLight();
  state = State::EsperandoIngresoNuevaClave;
}

void TurnOffLight()
{
  TurnOffEntranceLight();
  state = State::EsperandoIngresoNuevaClave;
}

void TurnOffLightA()
{
  TurnOffEntranceLight();
  state = State::ConfirmacionNuevaClave;
}

void TurnOffLightB()
{
  TurnOffEntranceLight();
  state = State::EsperandoIngresoClave;
}

void TurnOnLight()
{
  TurnOnEntranceLight();
  state = State::EsperandoIngresoNuevaClave;
}

void TurnOnLightA()
{
  TurnOnEntranceLight();
  state = State::ConfirmacionNuevaClave;
}

void TurnOnLightB()
{
  TurnOnEntranceLight();
  state = State::EsperandoIngresoClave;
}


void ExitInputNewPass()
{
  ShutdownScreen();
  TurnOffEntranceLight();
  ClearNewPassEnteredIntoLock();
  state = State::BloqueadoEsperandoClaveInicial;
}

void ClearInputNewPass()
{
  ReproduceKeyPressedSoundInBuzzer();
  ClearNewPassEnteredOnScreen();
  ClearNewPassEnteredIntoLock();
  state = State::EsperandoIngresoNuevaClave;
}

void ClearInputNewPassA()
{
  ReproduceKeyPressedSoundInBuzzer();
  ClearNewPassConfirmationEnteredOnScreen();
  ClearNewPassEnteredIntoLock();
  state = State::ConfirmacionNuevaClave;
}

void LoadNewPass()
{
  ReproduceKeyPressedSoundInBuzzer();
  ShowPasswordCharPressedOnScreen();
  LoadNewPasswordCharPressedIntoLock();
  state = State::EsperandoIngresoNuevaClave;
}

void LoadNewPassA()
{
  ReproduceKeyPressedSoundInBuzzer();
  ShowPasswordCharPressedOnScreen();
  LoadNewPasswordCharPressedIntoLock();
  state = State::ConfirmacionNuevaClave;
}

void ValidateNewPass()
{
  ReproduceKeyPressedSoundInBuzzer();
  StartNewPasswordConfirmation();
  InitializeScreenToInputPasswordConfirmation();
  state = State::ConfirmacionNuevaClave;
}

void ValidateNewPassA()
{
  ReproduceKeyPressedSoundInBuzzer();
  StartNewPasswordValidation();
  state = State::ValidacionNuevaClave;
}

void InvalidNewPass()
{
  ReproduceInvalidPassSoundInBuzzer();
  InitializeScreenToInputNewPassword();
  ClearNewPassEnteredIntoLock();
  state = State::EsperandoIngresoNuevaClave;
}

void ValidNewPass()
{
  ReproduceValidPassSoundInBuzzer();
  ShutdownScreen();
  TurnOffEntranceLight();
  state = State::BloqueadoEsperandoClaveInicial;
}

void InputPassWithLight()
{
  InitializeScreenToInputPassword();
  TurnOffEntranceLight();
  state = State::EsperandoIngresoClave;
}

void InputPassWithoutLight()
{
  InitializeScreenToInputPassword();
  TurnOnEntranceLight();
  state = State::EsperandoIngresoClave;
}

void UnlockWithButton()
{
  door_lock.UnlockWithButton();
  UnlockEntranceDoor();
  state = State::EsperandoAperturaPuertaBoton;
}

void ExitInputPass()
{
  ShutdownScreen();
  TurnOffEntranceLight();
  ClearPassEnteredIntoLock();
  state = State::BloqueadoEsperandoVisita;
}

void ClearInputPass()
{
  ReproduceKeyPressedSoundInBuzzer();
  ClearPassEnteredOnScreen();
  ClearPassEnteredIntoLock();
  state = State::EsperandoIngresoClave;
}

void LoadPass()
{
  ReproduceKeyPressedSoundInBuzzer();
  ShowPasswordCharPressedOnScreen();
  LoadPasswordCharPressedIntoLock();
  state = State::EsperandoIngresoClave;
}

void ValidatePass()
{
  ReproduceKeyPressedSoundInBuzzer();
  StartPasswordValidation();
  state = State::ValidacionClave;
}

void NotifTimeoutPass()
{
  state = State::EsperandoIngresoClave;
}

void InvalidPass()
{
  ReproduceInvalidPassSoundInBuzzer();
  ClearPassEnteredOnScreen();
  ClearPassEnteredIntoLock();
  state = State::EsperandoIngresoClave;
}

void ValidPass()
{
  ReproduceValidPassSoundInBuzzer();
  UnlockEntranceDoor();
  ShowValidPassMessageOnScreen();
  ClearPassEnteredIntoLock();
  state = State::EsperandoAperturaPuerta;
}

void BackToLocked()
{
  ShutdownScreen();
  TurnOffEntranceLight();
  ClearPassEnteredIntoLock();
  LockEntranceDoor();
  state = State::BloqueadoEsperandoVisita;
}

void BackToLockedA()
{
  ShutdownScreen();
  TurnOffEntranceLight();
  ClearPassEnteredIntoLock();
  LockEntranceDoor();
  ReproduceInvalidPassSoundInBuzzer();
  state = State::BloqueadoEsperandoVisita;
}

void BackToLockedB()
{
  LockEntranceDoor();
  state = State::BloqueadoEsperandoVisita;
}

void BackToLockedC()
{
  ShutdownScreen();
  TurnOffEntranceLight();
  ClearPassEnteredIntoLock();
  LockEntranceDoor();
  state = State::BloqueadoEsperandoVisita;
}

void WaitPerson()
{
  ShowOpenDoorMessageOnScreen();
  CancelDoorOpenTimer();
  state = State::EsperandoEntradaPersona;
}

void WaitPersonA()
{
  CancelDoorOpenTimer();
  state = State::EsperandoEntradaPersona;
}

void NotifOpenDoor()
{
  state = State::EsperandoEntradaPersona;
}

transition state_table[MAX_STATES][MAX_EVENTS] =
{
      {None       , CreatePass           ,  None              , None                , None                  , None                , None            , None                , None              , None            ,  None                    , None          , None        , None                  , None          , None          , None                    }, // state CerraduraInit
      {None       , None                 ,  None              , NewPassWithLight    , NewPassWithoutLight   , None                , None            , None                , None              , None            ,  None                    , None          , None        , None                  , None          , None          , None                    }, // state BloqueadoEsperandoClaveInicial
      {None       , None                 ,  None              , TurnOffLight        , TurnOnLight           , ExitInputNewPass    , None            , ClearInputNewPass   , LoadNewPass       , ValidateNewPass ,  None                    , None          , None        , None                  , None          , None          , None                    }, // state EsperandoIngresoNuevaClave
      {None       , None                 ,  None              , TurnOffLightA       , TurnOnLightA          , ExitInputNewPass    , None            , ClearInputNewPassA  , LoadNewPassA      , ValidateNewPassA,  None                    , None          , None        , None                  , None          , None          , None                    }, // state ConfirmacionNuevaClave
      {None       , None                 ,  None              , None                , None                  , None                , None            , None                , None              , None            ,  None                    , InvalidNewPass, ValidNewPass, None                  , None          , None          , None                    }, // state ValidacionNuevaClave
      {None       , None                 ,  None              , InputPassWithLight  , InputPassWithoutLight , None                , UnlockWithButton, None                , None              , None            ,  None                    , None          , None        , None                  , None          , None          , None                    }, // state BloqueadoEsperandoVisita
      {None       , None                 ,  None              , TurnOffLightB       , TurnOnLightB          , ExitInputPass       , None            , ClearInputPass      , LoadPass          , ValidatePass    ,  None                    , None          , None        , None                  , None          , None          , None                    }, // state EsperandoIngresoClave
      {None       , None                 ,  None              , None                , None                  , None                , None            , None                , None              , None            ,  NotifTimeoutPass        , InvalidPass   , ValidPass   , None                  , None          , None          , None                    }, // state ValidacionClave
      {None       , None                 ,  None              , None                , None                  , BackToLocked        , None            , None                , None              , None            ,  None                    , None          , None        , BackToLockedA         , WaitPerson    , None          , None                    }, // state EsperandoAperturaPuerta
      {None       , None                 ,  None              , None                , None                  , None                , None            , None                , None              , None            ,  None                    , None          , None        , BackToLockedB         , WaitPersonA   , None          , None                    }, // state EsperandoAperturaPuertaBoton
      {None       , None                 ,  None              , None                , None                  , None                , None            , None                , None              , None            ,  None                    , None          , None        , None                  , None          , BackToLockedC , NotifOpenDoor           }, // state EsperandoEntradaPersona
     //Continue   , ClaveNoConfigurada   ,  ClaveConfigurada  , PersonaDetectadaDia , PersonaDetectadaNoche , PersonaNoDetectada  , BotonPresionado , ClearClaveIngresada , CaracterIngresado , ValidarClave    ,  TimeOutValidacionClave  , ClaveInvalida , ClaveValida , TimeOutAperturaPuerta , SeAbrioPuerta , SeCerroPuerta , NotificarPuertaAbierta
};

void DoInit()
{
  Serial.begin(115200);

  //entrance_door.Setup();
  entrance_sensor.Setup();
  //door_sensor.Setup();
  //button.Setup();

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

    if (door_lock.CheckPasswordExistence() ||
        door_lock.CheckPasswordSettingInProgress() ||
        //button.CheckStatus() ||
        //door_lock.CheckUnlockInProgress() ||
        //door_lock.CheckOpeningDoorTimeout() ||
        keypad.CheckStatus() ||
        //door_sensor.CheckOpenedDoorTimer() ||
        //door_sensor.CheckClosedDoorTimer() ||
        //door_sensor.CheckStatus() ||
        entrance_sensor.CheckStatus())
    {
      return;
    }
  }

  event = Event::Continue;
}

void StateMachine()
{
  GenerateEvent();

  if( (event >= 0) && (event < MAX_EVENTS) && (state >= 0) && (state < MAX_STATES) )
  {
    if( event != Event::Continue )
    {
      DebugPrintEstado(states_s[state], events_s[event]);
    }
    
    state_table[state][event]();
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

// Utils
void InitializeScreenToInputPassword()
{
  lcd.TurnOnScreen();
  lcd.LoadInputPassScreen();
}

void InitializeScreenToInputNewPassword()
{
  lcd.TurnOnScreen();
  lcd.LoadNewPassScreen();
}

void InitializeScreenToInputPasswordConfirmation()
{
  lcd.TurnOnScreen();
  lcd.LoadConfirmNewPassScreen();
}

void ClearPassEnteredOnScreen()
{
  lcd.ResetInputPassScreen();
}

void ClearNewPassEnteredOnScreen()
{
  lcd.ResetNewPassScreen();
}

void ClearNewPassConfirmationEnteredOnScreen()
{
  lcd.ResetConfirmNewPassScreen();
}

void ShowPasswordCharPressedOnScreen()
{
  lcd.ShowKeyPressed(keypad.GetLastKeyPressed());
}

void ShowValidPassMessageOnScreen()
{
  lcd.ShowMessage("Clave correcta", "Empuje la puerta");
}

void ShutdownScreen()
{
  lcd.ClearScreen();
  lcd.TurnOffScreen();
}

void TurnOnEntranceLight()
{
  if (light.GetIsOn() == false)
  {
    light.TurnOn();
  }
}

void TurnOffEntranceLight()
{
  if (light.GetIsOn() == true)
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
  door_lock.LoadCharacter(keypad.GetLastKeyPressed());
}

void LoadNewPasswordCharPressedIntoLock()
{
  door_lock.LoadNewCharacter(keypad.GetLastKeyPressed());
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
  lcd.TurnOnScreen();
  lcd.ShowMessage("Puerta abierta", "pase");
}

void CancelDoorOpenTimer()
{
  door_lock.StopTimerPuerta();
}
