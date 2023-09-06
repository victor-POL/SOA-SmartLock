#include "main.h"

LCD lcd = LCD();
KeyPad keypad = KeyPad();
Lock lock = Lock("1A");
MyServo servo = MyServo();

void setup()
{
    Serial.begin(115200);

    lcd.setup();

    lcd.setupInputPassScreen();
    
    servo.setup();
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
    delay(10);
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
        servo.changeOrientation(179);
        lcd.showMessaggeInLine(0, "Ya puede empujar");
        lcd.showMessaggeInLine(1, "la puerta");
        Buzzer::activateSuccessSound();
    }
    else
    {
        lcd.showMessaggeInLine(0, "Contrasena");
        lcd.showMessaggeInLine(1, "incorrecta");
        Buzzer::activateFailSound();
    }
    delay(BUZZER_FAIL_DURATION);
    servo.changeOrientation(0);
    lock.resetPassEntered();
    lcd.resetInputPassScreen();
}

void optionNoInput()
{
    if (lcd.checkCursorInterval() == UPDATE_CURSOR && lock.getLengthPassEntered() <= MAX_PASSWORD_LENGTH)
    {
        lcd.updateInactiveCursor();
    }
}

void optionKeyPressed(char keyPressed)
{
    Buzzer::activateKeyPressedSound();
    lock.loadCharacter(keyPressed);
    lcd.showKeyPressed(keyPressed);
    Serial.println(lock.getPassEntered());
}
