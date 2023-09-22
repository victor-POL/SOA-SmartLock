#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <string>
#include "Connections.h"

#define MAX_LCD_LENGTH 16
#define CURSOR_INTERVAL 500
#define UPDATE_CURSOR 1

class LCD
{
private:
  LiquidCrystal_I2C screen;

  int cursor_pos;
  static LCD *instance;

  LCD() : screen(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS)
  {
    this->cursor_pos = 0;
  }

  int FindFirstNotSpace(const String &str)
  {
    int length = str.length();
    for (int i = 0; i < length; i++)
    {
      if (!isWhitespace(str.charAt(i)))
      {
        return i;
      }
    }
    return -1;
  }

public:
  static LCD *GetInstance()
  {
    if (instance == NULL)
    {
      instance = new LCD();
    }
    return instance;
  }

  void Setup()
  {
    this->screen.init();
  }

  void LoadInputPassScreen()
  {
    cursor_pos = 0;
    this->screen.clear();
    ShowMessage("Ingrese clave:", 0);
    this->screen.setCursor(0, 1);
  }

  void LoadNewPassScreen()
  {
    cursor_pos = 0;
    this->screen.clear();
    ShowMessage("Nueva clave:", 0);
    this->screen.setCursor(0, 1);
  }

  void LoadConfirmNewPassScreen()
  {
    cursor_pos = 0;
    this->screen.clear();
    ShowMessage("Confirma clave:", 0);
    this->screen.setCursor(0, 1);
  }

  void ResetInputPassScreen()
  {
    cursor_pos = 0;
    this->screen.setCursor(0, 1);
    for (int i = 0; i < MAX_LCD_LENGTH; i++)
    {
      this->screen.print(' ');
    }
    this->screen.setCursor(0, 1);
  }

  // Messages
  void ShowKeyPressed(char key_pressed)
  {
    this->screen.setCursor(cursor_pos, 1);
    this->screen.print(key_pressed);
    cursor_pos++;
    if (cursor_pos >= MAX_LCD_LENGTH)
    {
      cursor_pos = 0;
    }
  }

  void ShowMessage(String message1, String message2)
  {
    this->screen.clear();
    this->screen.setCursor(0, 0);
    this->screen.print(message1.c_str());
    this->screen.setCursor(0, 1);
    this->screen.print(message2.c_str());
  }

  void ShowMessage(String message, int line)
  {
    this->screen.setCursor(0, line);
    this->screen.print(message.c_str());
  }

  // Cursor

  void TurnOff()
  {
    this->screen.clear();
    this->screen.noBacklight();
  }

  void TurnOn()
  {
    this->screen.clear();
    this->screen.backlight();
  }

  void Clear()
  {
    this->screen.clear();
  }
};