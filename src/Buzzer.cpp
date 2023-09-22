#include <Arduino.h>
#include "Component.cpp"

#define STATUS_KEY_SOUND 10
#define STATUS_SUCESS_SOUND 11
#define STATUS_FAIL_SOUND 12
#define STATUS_NO_SOUND 13

#define BUZZER_KEY_DURATION 15
#define BUZZER_SUCCESS_DURATION 1000
#define BUZZER_FAIL_DURATION 2000

#define BUZZER_SUCCESS_FREQ 800
#define BUZZER_FAIL_FREQ 200
#define BUZZER_KEY_FREQ 500

#define BUZZER_CHANNEL 4
#define BUZZER_RESOLUTION 8
#define BUZZER_FREQUENCY 2000

class Buzzer : public Component
{
private:
  int status;
  int time_sound_activated;

  void StartTimer()
  {
    time_sound_activated = millis();
  }

  bool ReachedTimeout()
  {
    int current_time = millis();
    int time_elapsed = current_time - time_sound_activated;
    int limit = 0;

    switch (status)
    {
    case STATUS_KEY_SOUND:
      limit = BUZZER_KEY_DURATION;
      break;
    case STATUS_SUCESS_SOUND:
      limit = BUZZER_SUCCESS_DURATION;
      break;
    case STATUS_FAIL_SOUND:
      limit = BUZZER_FAIL_DURATION;
      break;
    }

    return time_elapsed > limit;
  }

public:
  Buzzer(int pin_selected) : Component(pin_selected)
  {
    this->status = STATUS_NO_SOUND;
    this->time_sound_activated = 0;
  }

  void Setup()
  {
    ledcSetup(BUZZER_CHANNEL, BUZZER_FREQUENCY, BUZZER_RESOLUTION);
    ledcAttachPin(this->pin_selected, BUZZER_CHANNEL);
  }

  void ActivateSuccessSound()
  {
    this->status = STATUS_SUCESS_SOUND;
    StartTimer();
    ledcWriteTone(BUZZER_CHANNEL, BUZZER_SUCCESS_FREQ);
  }

  void ActivateErrorSound()
  {
    this->status = STATUS_FAIL_SOUND;
    StartTimer();
    ledcWriteTone(BUZZER_CHANNEL, BUZZER_FAIL_FREQ);
  }

  void ActivateKeyPressedSound()
  {
    this->status = STATUS_KEY_SOUND;
    StartTimer();
    ledcWriteTone(BUZZER_CHANNEL, BUZZER_KEY_FREQ);
  }

  void DeactivateSound()
  {
    this->status = STATUS_NO_SOUND;
    ledcWriteTone(BUZZER_CHANNEL, 0);
  }

  bool CheckStatus()
  {
    if (status != STATUS_NO_SOUND && ReachedTimeout() == true)
    {
      DeactivateSound();
      status = STATUS_NO_SOUND;
      return true;
    }

    return false;
  }
};