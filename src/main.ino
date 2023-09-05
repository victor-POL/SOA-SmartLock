#include "main.h"
// Keypad
const int ROWS_KEYPAD = 4;
const int COLS_KEYPAD = 4;

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

Keypad keypad = Keypad(makeKeymap(keys), rowPins, columnPins, ROWS_KEYPAD, COLS_KEYPAD);

// Password
char passEntered[MAX_PASSWORD_LENGTH + 1];
char validPassword[] = "1234A";
int passPos = 0;

LCD lcd = LCD();

void setup()
{
    Serial.begin(115200);
    
    lcd.setupInputPassScreen();
}

void loop()
{
    char keyPressed = keypad.getKey();

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
    resetPassEntered();
    lcd.resetInputPassScreen();
}

void optionEnterKeyPressed()
{
    if (validatePassword(passEntered) == VALID_PASS)
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
    resetPassEntered();
    lcd.resetInputPassScreen();
}

void optionNoInput()
{
    if(lcd.checkCursorInterval() == UPDATE_CURSOR && strlen(passEntered) != MAX_PASSWORD_LENGTH)
    {
        lcd.updateInactiveCursor();
    }
}

void optionKeyPressed(char keyPressed)
{
    tone(BUZZER_PIN, BUZZER_KEY_FREQ, BUZZER_KEY_DURATION);
    loadPassword(keyPressed);
    lcd.showKeyPressed(keyPressed);
    Serial.println(passEntered);
}

// Password

bool validatePassword(char passEntered[])
{
    if (strcmp(passEntered, validPassword) == 0)
    {
        return VALID_PASS;
    }
    else
    {
        return INVALID_PASS;
    }
}

void resetPassEntered()
{
    passPos = 0;
    passEntered[0] = '\0';
}

void loadPassword(char keyPressed)
{
    if (strlen(passEntered) < 16)
    {
        passEntered[passPos] = keyPressed;
        passEntered[passPos + 1] = '\0';
        passPos++;
    }
}
