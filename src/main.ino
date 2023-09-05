#include "main.h"

LCD lcd = LCD();
KeyPad keypad = KeyPad();
Lock lock = Lock("1A");


void setup()
{
    Serial.begin(115200);
    
    lcd.setupInputPassScreen();
}

void loop()
{
    char keyPressed = keypad.getPressedKey();

    switch (keyPressed)
    {
    case KEY_CLEAR:
        optionClearKeyPressed();
        break;
    case KEY_ENTER:
        optionEnterKeyPressed();
        break;
    case NO_PRESSED_KEY:
        optionNoInput();
        break;
    default:
        optionKeyPressed(keyPressed);
    }
}

// Funciones
void optionClearKeyPressed()
{
    lock.resetPassEntered();
    lcd.resetInputPassScreen();
}

void optionEnterKeyPressed()
{
    if (lock.unlock() == VALID_PASS)
    {
        lcd.showMessaggeInLine(0, "Ya puede empujar");
        lcd.showMessaggeInLine(1, "la puerta");
        Buzzer::activateSuccessBuzzer();
    }
    else
    {
        lcd.showMessaggeInLine(0, "Contrasena");
        lcd.showMessaggeInLine(1, "incorrecta");
        Buzzer::activateFailBuzzer();
    }
    delay(BUZZER_FAIL_DURATION);
    lock.resetPassEntered();
    lcd.resetInputPassScreen();
}

void optionNoInput()
{
    if(lcd.checkCursorInterval() == UPDATE_CURSOR && lock.getLengthPassEntered() <= MAX_PASSWORD_LENGTH)
    {
        lcd.updateInactiveCursor();
    }
}

void optionKeyPressed(char keyPressed)
{
    tone(BUZZER_PIN, BUZZER_KEY_FREQ, BUZZER_KEY_DURATION);
    lock.loadCharacter(keyPressed);
    lcd.showKeyPressed(keyPressed);
    Serial.println(lock.getPassEntered());
}
