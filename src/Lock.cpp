#include <Arduino.h>
#include <string>
#include "Events.h"
#include "Storage.cpp"

#define MAX_PASSWORD_LENGTH 16
#define VALID_PASS true
#define INVALID_PASS false
#define UMBRAL_TIMEOUT_PUERTA 6000
#define DEFAULT_PASSWORD "A"

extern enum Events event;

class Locker
{
private:
  String passEntered;
  String validPassword;
  String newPasswordEntered;
  bool isLocked;
  bool unlockInProgress;
  bool newPassInProgress;
  bool checkTimeoutPuerta;
  bool passwordSetted;
  bool initSuccess;
  int lastCurrentTime;
  Storage storage;

  void StartTimer()
  {
    lastCurrentTime = millis();
  }

  bool ReachedTimeout()
  {
    int currentTime = millis();
    int timeElapsed = currentTime - lastCurrentTime;
    return timeElapsed > UMBRAL_TIMEOUT_PUERTA;
  }

public:
  Locker()
  {
    storage = Storage();
    validPassword = DEFAULT_PASSWORD;
    passwordSetted = false;
    initSuccess = false;
    isLocked = true;
    unlockInProgress = false;
    newPassInProgress = false;
    checkTimeoutPuerta = false;
    lastCurrentTime = -1;
  }

  void UnlockWithButton()
  {
    StartTimer();
    isLocked = false;
    checkTimeoutPuerta = true;
  }

  bool Unlock()
  {
    if (strcmp(passEntered.c_str(), validPassword.c_str()) == 0)
    {
      isLocked = false;
      checkTimeoutPuerta = true;
      StartTimer();
      return VALID_PASS;
    }
    else
    {
      return INVALID_PASS;
    }
  }

  void Lock()
  {
    isLocked = true;
  }

  void ResetPassEntered()
  {
    passEntered = "";
  }

  void ResetNewPassEntered()
  {
    newPasswordEntered = "";
  }

  void LoadCharacter(char keyPressed)
  {
    if (passEntered.length() < MAX_PASSWORD_LENGTH)
    {
      passEntered += keyPressed;
    }
  }

  void LoadNewCharacter(char keyPressed)
  {
    if (newPasswordEntered.length() < MAX_PASSWORD_LENGTH)
    {
      newPasswordEntered += keyPressed;
    }
  }

  String get_pass_entered()
  {
    return passEntered;
  }

  bool CheckStatus()
  {
    if (initSuccess == false)
    {
      validPassword = storage.ReadData("password", DEFAULT_PASSWORD);
      passwordSetted = strcmp(validPassword.c_str(), DEFAULT_PASSWORD) != 0;
      initSuccess = true;
      if (passwordSetted == false)
      {
        event = EVENTO_CLAVE_NO_CONFIGURADA;
        return true;
      }
      else
      {
        event = EVENTO_CLAVE_CONFIGURADA;
        return true;
      }
    }
    if (newPassInProgress == true)
    {
      newPassInProgress = false;
      if (newPasswordEntered == validPassword)
      {
        event = EVENTO_CLAVE_VALIDA;
        storage.StoreData("password", validPassword.c_str());
      }
      else
      {
        event = EVENTO_CLAVE_INVALIDA;
        validPassword = "";
      }
      return true;
    }
    if (unlockInProgress == true)
    {
      unlockInProgress = false;

      if (Unlock() == VALID_PASS)
      {
        event = EVENTO_CLAVE_VALIDA;
      }
      else
      {
        event = EVENTO_CLAVE_INVALIDA;
      }

      return true;
    }
    else if (isLocked == false && checkTimeoutPuerta == true && ReachedTimeout())
    {
      checkTimeoutPuerta = false;
      event = EVENTO_TIMEOUT_APERTURA_PUERTA;
      return true;
    }

    return false;
  }

  void ChangeUnlockInProgress(bool unlockInProgress)
  {
    this->unlockInProgress = unlockInProgress;
  }

  void SetCheckTimeoutPuerta(bool checkTimeoutPuerta)
  {
    this->checkTimeoutPuerta = checkTimeoutPuerta;
  }

  bool CheckPassword(String password)
  {
    return strcmp(password.c_str(), validPassword.c_str()) == 0;
  }

  void ToPasswordConfirmation()
  {
    validPassword = newPasswordEntered;
    newPasswordEntered = "";
  }

  void ChangeNewPassInProgress(bool newPassInProgress)
  {
    this->newPassInProgress = newPassInProgress;
  }
};