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
    int timeSoundActivated;

    void startTimer()
    {
        timeSoundActivated = millis();
    }

    bool reachedTimeout()
    {
        int currentTime = millis();
        int timeElapsed = currentTime - timeSoundActivated;
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

        return timeElapsed > limit;
    }

public:
    Buzzer(int pinSelected) : Component(pinSelected)
    {
        this->status = STATUS_NO_SOUND;
        this->timeSoundActivated = 0;
    }

    void setup()
    {
        ledcSetup(BUZZER_CHANNEL, BUZZER_FREQUENCY, BUZZER_RESOLUTION);
        ledcAttachPin(this->pinSelected, BUZZER_CHANNEL);
    }

    void activateSuccessSound()
    {
        this->status = STATUS_SUCESS_SOUND;
        startTimer();
        ledcWriteTone(BUZZER_CHANNEL, BUZZER_SUCCESS_FREQ);
    }

    void activateErrorSound()
    {
        this->status = STATUS_FAIL_SOUND;
        startTimer();
        ledcWriteTone(BUZZER_CHANNEL, BUZZER_FAIL_FREQ);
    }

    void activateKeyPressedSound()
    {
        this->status = STATUS_KEY_SOUND;
        startTimer();
        ledcWriteTone(BUZZER_CHANNEL, BUZZER_KEY_FREQ);
    }

    void deactivateSound()
    {
        this->status = STATUS_NO_SOUND;
        ledcWriteTone(BUZZER_CHANNEL, 0);
    }

    bool checkStatus()
    {
        if (status != STATUS_NO_SOUND && reachedTimeout() == true)
        {
            deactivateSound();
            status = STATUS_NO_SOUND;
            return true;
        }

        return false;
    }
};