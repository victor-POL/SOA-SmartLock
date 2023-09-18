#include <Arduino.h>
#include <Preferences.h>
#include <string>
#include "Events.h"

#define MAX_PASSWORD_LENGTH 16
#define VALID_PASS true
#define INVALID_PASS false
#define UMBRAL_TIMEOUT_PUERTA 6000

extern int event;

class Lock
{
private:
    String passEntered;
    String validPassword;
    bool isLocked;
    bool unlockInProgress;
    bool checkTimeoutPuerta;
    int lastCurrentTime;
    Preferences preferences;

    void startTimer()
    {
        lastCurrentTime = millis();
    }

    bool reachedTimeout()
    {
        int currentTime = millis();
        int timeElapsed = currentTime - lastCurrentTime;
        return timeElapsed > UMBRAL_TIMEOUT_PUERTA;
    }

public:
    Lock()
    {
        preferences.begin("smartLock", false);
        validPassword = preferences.getString("password", "A");
        isLocked = true;
        unlockInProgress = false;
        checkTimeoutPuerta = false;
        lastCurrentTime = -1;
    }

    bool unlock()
    {
        if (strcmp(passEntered.c_str(), validPassword.c_str()) == 0)
        {
            isLocked = false;
            checkTimeoutPuerta = true;
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
        if (validPassword == "A")
        {
            event = EVENTO_CLAVE_NO_CONFIGURADA;
            return true;
        }
        else
        {
            preferences.end();
        }
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
        else if (isLocked == false && checkTimeoutPuerta == true && reachedTimeout())
        {
            checkTimeoutPuerta = false;
            event = EVENTO_TIMEOUT_APERTURA_PUERTA;
            return true;
        }

        return false;
    }

    void changeUnlockInProgress(bool unlockInProgress)
    {
        this->unlockInProgress = unlockInProgress;
    }

    void setCheckTimeoutPuerta(bool checkTimeoutPuerta)
    {
        this->checkTimeoutPuerta = checkTimeoutPuerta;
    }

    void setValidPassword(String validPassword)
    {
        this->validPassword = validPassword;
        preferences.putString("password", validPassword);
    }

    bool checkPassword(String password)
    {
        return strcmp(password.c_str(), validPassword.c_str()) == 0;
    }
};