#include <Arduino.h>
#include <string>
#include "Events.h"
#include "Storage.cpp"

#define MAX_PASSWORD_LENGTH 16
#define VALID_PASS true
#define INVALID_PASS false
#define UMBRAL_TIMEOUT_PUERTA 6000
#define DEFAULT_PASSWORD "A"

extern enum Event event;

class Locker
{
private:
  String pass_entered;
  String valid_password;
  String new_password_entered;
  bool is_locked;
  bool unlock_in_progress;
  bool new_pass_in_progress;
  bool check_timeout_puerta;
  bool password_setted;
  bool init_success;
  int last_current_time;
  Storage storage;

  void StartTimer()
  {
    last_current_time = millis();
  }

  bool ReachedTimeout()
  {
    int currentTime = millis();
    int timeElapsed = currentTime - last_current_time;
    return timeElapsed > UMBRAL_TIMEOUT_PUERTA;
  }

public:
  Locker()
  {
    storage = Storage();
    valid_password = DEFAULT_PASSWORD;
    password_setted = false;
    init_success = false;
    is_locked = true;
    unlock_in_progress = false;
    new_pass_in_progress = false;
    check_timeout_puerta = false;
    last_current_time = -1;
  }

  void UnlockWithButton()
  {
    StartTimer();
    is_locked = false;
    check_timeout_puerta = true;
  }

  bool Unlock()
  {
    if (strcmp(pass_entered.c_str(), valid_password.c_str()) == 0)
    {
      is_locked = false;
      check_timeout_puerta = true;
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
    is_locked = true;
  }

  void ResetPassEntered()
  {
    pass_entered = "";
  }

  void ResetNewPassEntered()
  {
    new_password_entered = "";
  }

  void LoadCharacter(char key_pressed)
  {
    if (pass_entered.length() < MAX_PASSWORD_LENGTH)
    {
      pass_entered += key_pressed;
    }
  }

  void LoadNewCharacter(char key_pressed)
  {
    if (new_password_entered.length() < MAX_PASSWORD_LENGTH)
    {
      new_password_entered += key_pressed;
    }
  }

  String get_pass_entered()
  {
    return pass_entered;
  }

  bool CheckStatus()
  {
    if (init_success == false)
    {
      valid_password = storage.ReadData("password", DEFAULT_PASSWORD);
      password_setted = strcmp(valid_password.c_str(), DEFAULT_PASSWORD) != 0;
      init_success = true;
      if (password_setted == false)
      {
        event = Event::ClaveNoConfigurada;
        return true;
      }
      else
      {
        event = Event::ClaveConfigurada;
        return true;
      }
    }
    if (new_pass_in_progress == true)
    {
      new_pass_in_progress = false;
      if (new_password_entered == valid_password)
      {
        event = Event::ClaveValida;
        storage.StoreData("password", valid_password.c_str());
      }
      else
      {
        event = Event::ClaveInvalida;
        valid_password = "";
      }
      return true;
    }
    if (unlock_in_progress == true)
    {
      unlock_in_progress = false;

      if (Unlock() == VALID_PASS)
      {
        event = Event::ClaveValida;
      }
      else
      {
        event = Event::ClaveInvalida;
      }

      return true;
    }
    else if (is_locked == false && check_timeout_puerta == true && ReachedTimeout())
    {
      check_timeout_puerta = false;
      event = Event::TimeOutAperturaPuerta;
      return true;
    }

    return false;
  }

  void ChangeUnlockInProgress(bool unlock_in_progress)
  {
    this->unlock_in_progress = unlock_in_progress;
  }

  void SetCheckTimeoutPuerta(bool check_timeout_puerta)
  {
    this->check_timeout_puerta = check_timeout_puerta;
  }

  bool CheckPassword(String password)
  {
    return strcmp(password.c_str(), valid_password.c_str()) == 0;
  }

  void ToPasswordConfirmation()
  {
    valid_password = new_password_entered;
    new_password_entered = "";
  }

  void ChangeNewPassInProgress(bool new_pass_in_progress)
  {
    this->new_pass_in_progress = new_pass_in_progress;
  }
};