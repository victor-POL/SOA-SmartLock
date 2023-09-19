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

    int cursorPos;
    bool cursorVisible;
    unsigned long previousMillis;
    static LCD *instance;

    LCD() : screen(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS)
    {
        this->cursorPos = 0;
        this->cursorVisible = true;
        this->previousMillis = 0;
    }

    int findFirstNotSpace(const String &str)
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
    static LCD *getInstance()
    {
        if (instance == NULL)
        {
            instance = new LCD();
        }
        return instance;
    }

    void setup()
    {
        this->screen.init();
    }

    void loadInputPassScreen()
    {
        cursorPos = 0;
        this->screen.clear();
        showMessage("Ingrese clave:", 0);
        this->screen.setCursor(0, 1);
    }

    void loadNewPassScreen()
    {
        cursorPos = 0;
        this->screen.clear();
        showMessage("Nueva clave:", 0);
        this->screen.setCursor(0, 1);
    }

    void loadConfirmNewPassScreen()
    {
        cursorPos = 0;
        this->screen.clear();
        showMessage("Confirma clave:", 0);
        this->screen.setCursor(0, 1);
    }

    void resetInputPassScreen()
    {
        cursorPos = 0;
        this->screen.setCursor(0, 1);
        for (int i = 0; i < MAX_LCD_LENGTH; i++)
        {
            this->screen.print(' ');
        }
        this->screen.setCursor(0, 1);
    }

    // Messages
    void showKeyPressed(char keyPressed)
    {
        this->screen.setCursor(cursorPos, 1);
        this->screen.print(keyPressed);
        cursorPos++;
        if (cursorPos >= MAX_LCD_LENGTH)
        {
            cursorPos = 0;
        }
    }

    void showMessage(String message1, String message2)
    {
        this->screen.clear();
        this->screen.setCursor(0, 0);
        this->screen.print(message1.c_str());
        this->screen.setCursor(0, 1);
        this->screen.print(message2.c_str());
    }

    void showMessage(String message, int line)
    {
        this->screen.setCursor(0, line);
        this->screen.print(message.c_str());
    }

    // Cursor

    int checkCursorInterval()
    {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= CURSOR_INTERVAL)
        {
            previousMillis = currentMillis;
            cursorVisible = !cursorVisible;
            return UPDATE_CURSOR;
        }
        return 0;
    }

    void updateCursor()
    {
        if (cursorVisible)
        {
            this->screen.setCursor(cursorPos, 1);
            this->screen.print('_');
        }
        else
        {
            this->screen.setCursor(cursorPos, 1);
            this->screen.print(' ');
        }
    }

    void turnOff()
    {
        this->screen.clear();
        this->screen.noBacklight();
    }

    void turnOn()
    {
        this->screen.clear();
        this->screen.backlight();
    }

    void clear()
    {
        this->screen.clear();
    }
};