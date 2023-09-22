#include <Arduino.h>
#include <Keypad.h>
#include "States.h"
#include "Events.h"
#include "Connections.h"

#define NO_PRESSED_KEY 0
#define KEY_CLEAR '*'
#define KEY_ENTER '#'
#define ROWS_KEYPAD 4
#define COLS_KEYPAD 3
#define KEYS_ROW_1 '1', '2', '3'
#define KEYS_ROW_2 '4', '5', '6'
#define KEYS_ROW_3 '7', '8', '9'
#define KEYS_ROW_4 '*', '0', '#'

extern enum Event event;

class KeyPad
{
private:
  Keypad keypad;

  char keys[ROWS_KEYPAD][COLS_KEYPAD] = {
      {KEYS_ROW_1},
      {KEYS_ROW_2},
      {KEYS_ROW_3},
      {KEYS_ROW_4}};

  uint8_t row_pins[ROWS_KEYPAD] = {
      KEYPAD_ROW_1_PIN,
      KEYPAD_ROW_2_PIN,
      KEYPAD_ROW_3_PIN,
      KEYPAD_ROW_4_PIN,
  };
  uint8_t column_pins[COLS_KEYPAD] = {
      KEYPAD_COLUMN_1_PIN,
      KEYPAD_COLUMN_2_PIN,
      KEYPAD_COLUMN_3_PIN,
  };

  char last_key_pressed;

public:
  KeyPad() : keypad(makeKeymap(keys), row_pins, column_pins, ROWS_KEYPAD, COLS_KEYPAD)
  {
    this->last_key_pressed = NO_PRESSED_KEY;
  }

  char GetPressedKey()
  {
    return this->keypad.getKey();
  }

  bool CheckStatus()
  {
    char actual_pressed_key = GetPressedKey();

    if (actual_pressed_key != NO_PRESSED_KEY)
    {
      last_key_pressed = actual_pressed_key;

      switch (actual_pressed_key)
      {
      case KEY_CLEAR:
        event = Event::ClearClaveIngresada;
        break;
      case KEY_ENTER:
        event = Event::ValidarClave;
        break;
      default:
        event = Event::CaracterIngresado;
      }

      return true;
    }

    return false;
  }

  char get_last_key_pressed()
  {
    return last_key_pressed;
  }
};