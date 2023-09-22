#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "Connections.h"

#define MAX_LCD_LENGTH 16

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

  void SetCursor(int x, int y)
  {
    this->screen.setCursor(x, y);
  }

  void Print(String message)
  {
    this->screen.print(message.c_str());
  }

  void TurnOn()
  {
    this->screen.backlight();
  }

  void TurnOff()
  {
    this->screen.noBacklight();
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

  // Predefinen screens
  void LoadInputPassScreen()
  {
    this->cursor_pos = 0;
    ClearScreen();
    ShowMessage("Ingrese clave:", 0);
    SetCursor(0, 1);
  }

  void LoadNewPassScreen()
  {
    this->cursor_pos = 0;
    ClearScreen();
    ShowMessage("Nueva clave:", 0);
    SetCursor(0, 1);
  }

  void LoadConfirmNewPassScreen()
  {
    this->cursor_pos = 0;
    ClearScreen();
    ShowMessage("Confirma clave:", 0);
    SetCursor(0, 1);
  }

  void ResetInputPassScreen()
  {
    this->cursor_pos = 0;
    LoadInputPassScreen();
  }

  // Messages
  void ShowKeyPressed(char key_pressed)
  {
    SetCursor(this->cursor_pos, 1);
    Print((String)key_pressed);
    this->cursor_pos++;
    if (this->cursor_pos >= MAX_LCD_LENGTH)
    {
      this->cursor_pos = 0;
    }
  }

  void ShowMessage(String first_message, String second_message)
  {
    ClearScreen();
    SetCursor(0, 0);
    Print(first_message);
    SetCursor(0, 1);
    Print(second_message);
  }

  void ShowMessage(String message, int line)
  {
    SetCursor(0, line);
    Print(message);
  }

  // Actions
  void TurnOffScreen()
  {
    ClearScreen();
    TurnOff();
  }

  void TurnOnScreen()
  {
    ClearScreen();
    TurnOn();
  }

  void ClearScreen()
  {
    this->screen.clear();
  }
};