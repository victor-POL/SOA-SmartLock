#include <Arduino.h>

#define BUZZER_PIN 27

#define BUZZER_KEY_STATUS 10
#define BUZZER_SUCCESS_STATUS 11
#define BUZZER_FAIL_STATUS 12
#define BUZZER_NO_SOUND_STATUS 13

#define BUZZER_KEY_DURATION 15
#define BUZZER_SUCCESS_DURATION 3000
#define BUZZER_FAIL_DURATION 3000

#define BUZZER_SUCCESS_FREQ 800
#define BUZZER_FAIL_FREQ 200
#define BUZZER_KEY_FREQ 500

#define BUZZER_CHANNEL 4
#define BUZZER_RESOLUTION 8
#define BUZZER_FREQUENCY 2000

class Buzzer
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

        switch(status)
        {
            case BUZZER_KEY_STATUS:
                limit = BUZZER_KEY_DURATION;
                break;
            case BUZZER_SUCCESS_STATUS:
                limit = BUZZER_SUCCESS_DURATION;
                break;
            case BUZZER_FAIL_STATUS:
                limit = BUZZER_FAIL_DURATION;
                break;
        }

        return timeElapsed > limit;
    }

public:
    Buzzer()
    {
        status = -1;
        timeSoundActivated = 0;
    }

    void setup()
    {
        ledcSetup(BUZZER_CHANNEL, BUZZER_FREQUENCY, BUZZER_RESOLUTION);
        ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
    }

    void activateSuccessSound()
    {
        status = BUZZER_SUCCESS_STATUS;
        startTimer();
        ledcWriteTone(BUZZER_CHANNEL, BUZZER_SUCCESS_FREQ);
    }

    void activateErrorSound()
    {
        status = BUZZER_FAIL_STATUS;
        startTimer();
        ledcWriteTone(BUZZER_CHANNEL, BUZZER_FAIL_FREQ);
    }

    void activateKeyPressedSound()
    {
        status = BUZZER_KEY_STATUS;
        startTimer();
        ledcWriteTone(BUZZER_CHANNEL, BUZZER_KEY_FREQ);
    }

    void deactivateSound()
    {
        status = -1;
        ledcWriteTone(BUZZER_CHANNEL, 0);
    }

    bool checkStatus()
    {
        if (status != BUZZER_NO_SOUND_STATUS && reachedTimeout() == true)
        {
            deactivateSound();
            status = BUZZER_NO_SOUND_STATUS;
            return true;
        }

        return false;
    }
};