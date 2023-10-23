#include <Arduino.h>
#include "rgb_lcd.h"
#include "Connections.h"

class LCDRgb
{
private:
  rgb_lcd screen;

  int cursor_pos;
  static LCDRgb *instance;

  LCDRgb() : screen()
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
    this->screen.display();
  }

  void TurnOff()
  {
    this->screen.noDisplay();
  }

public:
  static LCDRgb *GetInstance()
  {
    if (instance == NULL)
    {
      instance = new LCDRgb();
    }
    return instance;
  }

  void Setup()
  {
    this->screen.begin(LCD_COLUMNS, LCD_ROWS);
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