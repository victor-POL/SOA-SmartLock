#include <Arduino.h>
#include <Keypad.h>
#include "States.h"
#include "Events.h"
#include "Connections.h"

#define NO_PRESSED_KEY 0
#define KEY_CLEAR '*'
#define KEY_ENTER '#'
#define ROWS_KEYPAD 4
#define COLS_KEYPAD 4
#define KEYS_ROW_1 '1', '2', '3', 'A'
#define KEYS_ROW_2 '4', '5', '6', 'B'
#define KEYS_ROW_3 '7', '8', '9', 'C'
#define KEYS_ROW_4 '*', '0', '#', 'D'

extern int event;

class KeyPad
{
private:
    Keypad keypad;

    char keys[ROWS_KEYPAD][COLS_KEYPAD] = {
        {KEYS_ROW_1},
        {KEYS_ROW_2},
        {KEYS_ROW_3},
        {KEYS_ROW_4}};

    uint8_t rowPins[ROWS_KEYPAD] = {
        KEYPAD_ROW_1_PIN,
        KEYPAD_ROW_2_PIN,
        KEYPAD_ROW_3_PIN,
        KEYPAD_ROW_4_PIN,
    };
    uint8_t columnPins[COLS_KEYPAD] = {
        KEYPAD_COLUMN_1_PIN,
        KEYPAD_COLUMN_2_PIN,
        KEYPAD_COLUMN_3_PIN,
        KEYPAD_COLUMN_4_PIN,
    };

    char lastKeyPressed;

public:
    KeyPad() : keypad(makeKeymap(keys), rowPins, columnPins, ROWS_KEYPAD, COLS_KEYPAD)
    {
        this->lastKeyPressed = NO_PRESSED_KEY;
    }

    char getPressedKey()
    {
        return this->keypad.getKey();
    }

    bool checkStatus()
    {
        char actualPressedKey = getPressedKey();

        if (actualPressedKey != NO_PRESSED_KEY)
        {
            lastKeyPressed = actualPressedKey;

            switch (actualPressedKey)
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

        return false;
    }

    char getLastKeyPressed()
    {
        return lastKeyPressed;
    }
};