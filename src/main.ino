#include "main.h"
#define TRIGGER_PIN_SENSOR_1 32
#define TRIGGER_PIN_SENSOR_2 33
#define ECHO_PIN_SENSOR_1 34
#define ECHO_PIN_SENSOR_2 35
#define RELAY_PIN 14
#define LIGHT_SEN_PIN 39

LCD* LCD::instance = NULL;
LCD lcd = *LCD::getInstance();
KeyPad keypad = KeyPad();
Lock lock = Lock("1A");
MyServo servo = MyServo();
Ultrasonic sensor1 = Ultrasonic(TRIGGER_PIN_SENSOR_1, ECHO_PIN_SENSOR_1);
Ultrasonic sensor2 = Ultrasonic(TRIGGER_PIN_SENSOR_2, ECHO_PIN_SENSOR_2);
const float GAMMA = 0.7;
const float RL10 = 50;

void setup()
{
  Serial.begin(115200);

  lcd.setup();

  lcd.setupInputPassScreen();

  servo.setup();

  sensor1.setup();
  sensor2.setup();

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LIGHT_SEN_PIN, INPUT);
}

void loop()
{
  char keyPressed = keypad.getPressedKey();


  // Distance
  float distance1 = sensor1.getDistance();
  float distance2 = sensor2.getDistance();


  if(distance1 < 30)
  {
    digitalWrite(RELAY_PIN, HIGH);
  }
  else
  {
    digitalWrite(RELAY_PIN, LOW);
  }

  // Light
  int analogValue = analogRead(LIGHT_SEN_PIN);
  float voltage = analogValue / 1024. * 5;
  float resistance = 2000 * voltage / (1 - voltage / 5);
  float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));

  Serial.println(analogValue);

  // Keypad
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
