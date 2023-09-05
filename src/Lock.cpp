#include <Arduino.h>
#include <string>

#define MAX_PASSWORD_LENGTH 16
#define VALID_PASS true
#define INVALID_PASS false

class Lock
{
private:
    String passEntered;
    String validPassword;
    int passPos;

public:
    Lock(String validPassword)
    {
        this->validPassword = validPassword;
        passPos = 0;
    }

    bool getLengthPassEntered()
    {
        return passEntered.length();
    }

    bool unlock()
    {
        if (passEntered == validPassword)
        {
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
};