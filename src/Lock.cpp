#include <Arduino.h>
#include <string>
#include "Events.h"
#include "Storage.cpp"

#define MAX_PASSWORD_LENGTH 16
#define VALID_PASS true
#define INVALID_PASS false
#define UMBRAL_TIMEOUT_PUERTA 6000
#define DEFAULT_PASSWORD "A"

extern int event;

class Lock
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

    void unlockWithButton()
    {
        startTimer();
        isLocked = false;
        checkTimeoutPuerta = true;
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

    void resetNewPassEntered()
    {
        newPasswordEntered = "";
    }

    void loadCharacter(char keyPressed)
    {
        if (passEntered.length() < MAX_PASSWORD_LENGTH)
        {
            passEntered += keyPressed;
        }
    }

    void loadNewCharacter(char keyPressed)
    {
        if (newPasswordEntered.length() < MAX_PASSWORD_LENGTH)
        {
            newPasswordEntered += keyPressed;
        }
    }

    String getPassEntered()
    {
        return passEntered;
    }

    bool checkStatus()
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

    bool checkPassword(String password)
    {
        return strcmp(password.c_str(), validPassword.c_str()) == 0;
    }

    void toPasswordConfirmation()
    {
        validPassword = newPasswordEntered;
        newPasswordEntered = "";
    }

    void changeNewPassInProgress(bool newPassInProgress)
    {
        this->newPassInProgress = newPassInProgress;
    }
};