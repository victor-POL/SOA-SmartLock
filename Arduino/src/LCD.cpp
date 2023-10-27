#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "Connections.h"

class LCD
{
private:
  LiquidCrystal_I2C screen;

  int cursor_pos;
  static LCD *instance;

  LCD() : screen(LCD_ADDR, LCD_COLUMNS, LCD_ROWS)
  {
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
    ClearScreen();
    ShowMessage("Ingrese clave:", 0);
    SetCursor(0, 1);
  }

  void LoadNewPassScreen()
  {
    ClearScreen();
    ShowMessage("Nueva clave:", 0);
    SetCursor(0, 1);
  }

  void LoadConfirmNewPassScreen()
  {
    ClearScreen();
    ShowMessage("Confirma clave:", 0);
    SetCursor(0, 1);
  }

  void LoadSucessUnlockScreen()
  {
    ClearScreen();
    ShowMessage("Clave correcta", "Empuje la puerta");
  }

  void LoadOpenDoorScreen()
  {
    ClearScreen();
    ShowMessage("Puerta abierta", "pase");
  }

  void ResetInputPassScreen()
  {
    LoadInputPassScreen();
  }

  void ResetNewPassScreen()
  {
    LoadNewPassScreen();
  }

  void ResetConfirmNewPassScreen()
  {
    LoadConfirmNewPassScreen();
  }

  // Messages
  void ShowKeyPressed(char key_pressed)
  {
    Print((String)key_pressed);
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