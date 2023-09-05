#include <Arduino.h>
#include <Keypad.h>

#define NO_PRESSED_KEY 0
#define KEY_CLEAR '*'
#define KEY_ENTER '#'
#define ROWS_KEYPAD 4
#define COLS_KEYPAD 4

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
        19,
        18,
        5,
        17};
    uint8_t columnPins[COLS_KEYPAD] = {
        16,
        4,
        2,
        15};

public:
    KeyPad()
    {
    }

    char getPressedKey()
    {
        return this->keypad.getKey();
    }
};