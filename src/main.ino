#include <Keypad.h>
#include <LiquidCrystal.h>
#include <ESP32Servo.h>

#define MAX_PASSWORD_LENGTH 16
#define NO_PRESSED_KEY 0
#define KEY_CLEAR '*'
#define KEY_ENTER '#'
#define MAX_LCD_LENGTH 16
#define CURSOR_INTERVAL 500
#define BUZZER_KEY_DURATION 15
#define BUZZER_SUCCESS_DURATION 3000
#define BUZZER_FAIL_DURATION 3000
#define BUZZER_PIN 12
#define BUZZER_SUCCESS_FREQ 1661
#define BUZZER_FAIL_FREQ 110
#define BUZZER_KEY_FREQ 494
#define SERVO_PIN 23

#define VALID_PASS true
#define INVALID_PASS false

// Keypad
const uint8_t ROWS_KEYPAD = 4;
const uint8_t COLS_KEYPAD = 4;

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

// LCD
LiquidCrystal lcd(14, 27, 26, 25, 33, 32);
int cursorPos = 0;
bool cursorVisible = true;
unsigned long previousMillis = 0;

// Buzzer
int canal = 0, frec = 2000, resolucion = 8;
unsigned long previousMillisBuzzer = 0;

// Password
char passEntered[MAX_PASSWORD_LENGTH + 1];
char validPassword[] = "1234A";
int passPos = 0;

// Servo

void setup()
{
    Serial.begin(115200);
    lcd.begin(16, 2);

    setupInputPassScreen();

    setupBuzzer();
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
    resetInputPassScreen();
}

void optionEnterKeyPressed()
{
    if (validatePassword(passEntered) == VALID_PASS)
    {
        showSuccessLoginMessage();
        activateSuccessBuzzer();
    }
    else
    {
        showErrorLoginMessage();
        activateFailBuzzer();
    }
    delay(BUZZER_FAIL_DURATION);
    resetPassEntered();
    resetInputPassScreen();
}

void optionNoInput()
{
    checkCursorInterval();
}

void optionKeyPressed(char keyPressed)
{
    tone(BUZZER_PIN, BUZZER_KEY_FREQ, BUZZER_KEY_DURATION);
    loadPassword(keyPressed);
    showKeyPressed(keyPressed);
    Serial.println(passEntered);
}

// LCD

void showSuccessLoginMessage()
{
    lcd.setCursor(0, 0);
    lcd.write("Ya puede empujar");
    lcd.setCursor(0, 1);
    lcd.write("la puerta");
}

void showErrorLoginMessage()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write("Contrasena");
    lcd.setCursor(0, 1);
    lcd.write("incorrecta");
}

void showKeyPressed(char keyPressed)
{
    lcd.setCursor(cursorPos, 1);
    lcd.print(keyPressed);
    cursorPos++;
    if (cursorPos >= MAX_LCD_LENGTH)
    {
        cursorPos = 0;
    }
}

void resetInputPassScreen()
{
    cursorPos = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ingrese clave:");
    lcd.setCursor(0, 1);
}

void checkCursorInterval()
{
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= CURSOR_INTERVAL)
    {
        previousMillis = currentMillis;
        cursorVisible = !cursorVisible;
        if (strlen(passEntered) != MAX_PASSWORD_LENGTH)
        {
            updateInactiveCursor();
        }
    }
}

void updateInactiveCursor()
{
    if (cursorVisible)
    {
        lcd.setCursor(cursorPos, 1);
        lcd.print('_');
    }
    else
    {
        lcd.setCursor(cursorPos, 1);
        lcd.print(' ');
    }
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

void createPassword()
{
    lcd.setCursor(0, 0);
    lcd.print("Nueva clave:");
    lcd.setCursor(0, 1);
    checkCursorInterval();
}

// Buzzer

void activateSuccessBuzzer()
{
    tone(BUZZER_PIN, BUZZER_SUCCESS_FREQ, BUZZER_SUCCESS_DURATION);
}

void activateFailBuzzer()
{
    tone(BUZZER_PIN, BUZZER_FAIL_FREQ, BUZZER_FAIL_DURATION);
}

// Setups

void setupInputPassScreen()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ingrese clave:");
    lcd.setCursor(0, 1);
}

void setupBuzzer()
{
    ledcSetup(canal, frec, resolucion);
    ledcAttachPin(BUZZER_PIN, 0);
}
