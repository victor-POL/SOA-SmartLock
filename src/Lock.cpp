#include <Arduino.h>
#include <string>
#include "Events.h"

#define MAX_PASSWORD_LENGTH 16
#define VALID_PASS true
#define INVALID_PASS false

extern int event;

class Lock
{
private:
    String passEntered;
    String validPassword;
    int passPos;
    bool isLocked;
    bool unlockInProgress;

public:
    Lock(String validPassword="A")
    {
        this->validPassword = validPassword;
        passPos = 0;
        isLocked = true;
        unlockInProgress = false;
    }

    bool getLengthPassEntered()
    {
        return passEntered.length();
    }

    bool unlock()
    {
        if (strcmp(passEntered.c_str(), validPassword.c_str()) == 0)
        {
            isLocked = false;
            return VALID_PASS;
        }
        else
        {
            return INVALID_PASS;
        }
    }

    void resetPassEntered()
    {
        passPos = 0;
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
        if(unlockInProgress == true)
        {
            unlockInProgress = false;

            if(unlock() == VALID_PASS)
            {
                event = EVENTO_CLAVE_VALIDA;
            }
            else
            {
                event = EVENTO_CLAVE_INVALIDA;
            }

            return true;
        }

        return false;
    }

    void changeUnlockInProgress(bool unlockInProgress)
    {
        this->unlockInProgress = unlockInProgress;
    }
};