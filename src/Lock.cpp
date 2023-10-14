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

class Lock
{
private:
  String pass_entered;
  String valid_password;
  String new_password_entered;
  bool is_locked;
  bool unlock_in_progress;
  bool new_pass_in_progress;
  bool check_timeout_puerta;
  bool init_success;
  int last_current_time;
  Storage storage;

  void StartTimer()
  {
    this->last_current_time = millis();
  }

  bool ReachedTimeout()
  {
    int current_time = millis();
    int time_elapsed = current_time - this->last_current_time;
    return time_elapsed > UMBRAL_TIMEOUT_PUERTA;
  }

  bool Unlock()
  {
    if (this->pass_entered == this->valid_password)
    {
      StartTimer();
      this->is_locked = false;
      this->check_timeout_puerta = true;
      return VALID_PASS;
    }
    else
    {
      return INVALID_PASS;
    }
  }

public:
  Lock()
  {
    this->storage = Storage();
    this->valid_password = DEFAULT_PASSWORD;
    this->init_success = false;
    this->is_locked = true;
    this->unlock_in_progress = false;
    this->new_pass_in_progress = false;
    this->check_timeout_puerta = false;
    this->last_current_time = -1;
  }

  // Actions
  void UnlockWitoutPass()
  {
    StartTimer();
    this->is_locked = false;
    this->check_timeout_puerta = true;
  }

  // Pass
  void ResetPassEntered()
  {
    this->pass_entered = "";
  }

  void ResetNewPassEntered()
  {
    this->new_password_entered = "";
  }

  void LoadCharacter(char key_pressed)
  {
    if (this->pass_entered.length() < MAX_PASSWORD_LENGTH)
    {
      this->pass_entered += key_pressed;
    }
  }

  void LoadNewCharacter(char key_pressed)
  {
    if (this->new_password_entered.length() < MAX_PASSWORD_LENGTH)
    {
      this->new_password_entered += key_pressed;
    }
  }

  // Status
  bool CheckPasswordExistence()
  {
    if (this->init_success == false)
    {
      this->init_success = true;
      this->valid_password = storage.ReadData("password", DEFAULT_PASSWORD);
      event = this->valid_password == DEFAULT_PASSWORD ? Event::ClaveNoConfigurada : Event::ClaveConfigurada;
      return true;
    }
    return false;
  }

  bool CheckPasswordSettingInProgress()
  {
    if (this->new_pass_in_progress == true)
    {
      this->new_pass_in_progress = false;
      if (this->new_password_entered == this->valid_password)
      {
        event = Event::ClaveValida;
        storage.StoreData("password", valid_password.c_str());
      }
      else
      {
        event = Event::ClaveInvalida;
        this->valid_password = "";
      }
      return true;
    }
    return false;
  }

  bool CheckUnlockInProgress()
  {
    if (this->unlock_in_progress == true)
    {
      this->unlock_in_progress = false;
      event = Unlock() == VALID_PASS ? Event::ClaveValida : Event::ClaveInvalida;
      return true;
    }
    return false;
  }

  bool CheckOpeningDoorTimeout()
  {
    if (this->is_locked == false && this->check_timeout_puerta == true && ReachedTimeout())
    {
      this->check_timeout_puerta = false;
      event = Event::TimeOutAperturaPuerta;
      return true;
    }
    return false;
  }

  // Utils
  void ChangeUnlockInProgress(bool unlock_in_progress)
  {
    this->unlock_in_progress = unlock_in_progress;
  }

  void ToPasswordConfirmation()
  {
    this->valid_password = this->new_password_entered;
    this->new_password_entered = "";
  }

  void ChangeNewPassInProgress(bool new_pass_in_progress)
  {
    this->new_pass_in_progress = new_pass_in_progress;
  }

  void StopTimerPuerta()
  {
    this->check_timeout_puerta = false;
  }
};