#include <Arduino.h>
#include <Keypad.h>
#include "States.h"
#include "Events.h"

#define NO_PRESSED_KEY 0
#define KEY_CLEAR '*'
#define KEY_ENTER '#'
#define ROWS_KEYPAD 4
#define COLS_KEYPAD 4

extern int event;

class KeyPad
{
private:
    Keypad keypad = Keypad(makeKeymap(keys), rowPins, columnPins, ROWS_KEYPAD, COLS_KEYPAD);

    char keys[ROWS_KEYPAD][COLS_KEYPAD] = {
        {'1',
         '2',
         '3',
         'A'},
        {'4',
         '5',
         '6',
         'B'},
        {'7',
         '8',
         '9',
         'C'},
        {'*',
         '0',
         '#',
         'D'}};

   uint8_t rowPins[ROWS_KEYPAD] = {
        13,
        14,
        12,
        17};
    uint8_t columnPins[COLS_KEYPAD] = {
        16,
        4,
        2,
        15}; 

    char lastKeyPressed = NO_PRESSED_KEY;

public:
    KeyPad()
    {
    }

    char getPressedKey()
    {
        return this->keypad.getKey();
    }

    bool checkStatus()
    {
        char actualPressedKey = getPressedKey();

        if(actualPressedKey != NO_PRESSED_KEY)
        {
            lastKeyPressed = actualPressedKey;

            switch(actualPressedKey)
            {
                case KEY_CLEAR:
                    event = EVENTO_CLEAR_CLAVE_INGRESADA;
                    break;
                case KEY_ENTER:
                    event = EVENTO_VALIDAR_CLAVE;
                    break;
                default:
                    event = EVENTO_CARACTER_INGRESADO;
            }

            return true;
        }

        return false;;
    }

    char getLastKeyPressed()
    {
        return lastKeyPressed;
    }
};