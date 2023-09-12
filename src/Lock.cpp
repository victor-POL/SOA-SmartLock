#include <Arduino.h>
#include <string>
#include "Events.h"

#define MAX_PASSWORD_LENGTH 16
#define VALID_PASS true
#define INVALID_PASS false
#define UMBRAL_TIMEOUT_PUERTA 10000

extern int event;

class Lock
{
private:
    String passEntered;
    String validPassword;
    bool isLocked;
    bool unlockInProgress;
    int timeUnlocked;

    void startTimer()
    {
        timeUnlocked = millis();
    }

    bool reachedTimeout()
    {
        int currentTime = millis();
        int timeElapsed = currentTime - timeUnlocked;
        return timeElapsed > UMBRAL_TIMEOUT_PUERTA;
    }

public:
    Lock(String validPassword = "A")
    {
        this->validPassword = validPassword;
        isLocked = true;
        unlockInProgress = false;
        timeUnlocked = 0;
    }

    bool unlock()
    {
        if (strcmp(passEntered.c_str(), validPassword.c_str()) == 0)
        {
            isLocked = false;
            startTimer();
            return VALID_PASS;
        }
        else
        {
            return INVALID_PASS;
        }
    }

    void lock()
    {
        isLocked = true;
    }

    void resetPassEntered()
    {
        passEntered = "";
    }

    void loadCharacter(char keyPressed)
    {
        if (passEntered.length() < MAX_PASSWORD_LENGTH)
        {
            passEntered += keyPressed;
        }
    }

    String getPassEntered()
    {
        return passEntered;
    }

    bool checkStatus()
    {
        if (unlockInProgress == true)
        {
            unlockInProgress = false;

            if (unlock() == VALID_PASS)
            {
                event = EVENTO_CLAVE_VALIDA;
            }
            else
            {
                event = EVENTO_CLAVE_INVALIDA;
            }

            return true;
        }
        else if (isLocked == false && reachedTimeout())
        {
            event = TIMEOUT_APERTURA_PUERTA;
            return true;
        }

        return false;
    }

    void changeUnlockInProgress(bool unlockInProgress)
    {
        this->unlockInProgress = unlockInProgress;
    }
};