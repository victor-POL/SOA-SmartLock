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
        showMessaggeInLine(0, "Ingrese clave:");
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
    void showMessageFullScreen(String message)
    {
        int length = message.length();
        int startPos = 0;

        this->screen.clear();
        this->screen.setCursor(0, 0);

        while (startPos < length)
        {
            int firstNoSpace = findFirstNotSpace(message.substring(startPos));

            if (firstNoSpace != -1)
            {
                int lineLength = std::min(MAX_LCD_LENGTH, length - (startPos + firstNoSpace));

                String line = message.substring(startPos + firstNoSpace, startPos + firstNoSpace + lineLength);

                if (line.length() > MAX_LCD_LENGTH)
                {
                    line = line.substring(0, MAX_LCD_LENGTH);
                }

                this->screen.print(line.c_str());
                this->screen.setCursor(0, 1);
                startPos += firstNoSpace + lineLength;
            }
        }
    }

    void showMessaggeInLine(int line, String message)
    {
        int length = message.length();

        this->screen.setCursor(0, line);

        for (int i = 0; i < MAX_LCD_LENGTH; i++)
        {
            this->screen.print(' ');
        }

        this->screen.setCursor(0, line);

        int firstNoSpace = findFirstNotSpace(message);

        if (firstNoSpace != -1)
        {
            int lineLength = std::min(MAX_LCD_LENGTH, length - firstNoSpace);

            String line = message.substring(firstNoSpace, firstNoSpace + lineLength);

            if (line.length() > MAX_LCD_LENGTH)
            {
                line = line.substring(0, MAX_LCD_LENGTH);
            }

            this->screen.print(line.c_str());
        }
    }

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