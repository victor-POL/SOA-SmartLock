#include <Arduino.h>

#define BUZZER_KEY_DURATION 15
#define BUZZER_SUCCESS_DURATION 3000
#define BUZZER_FAIL_DURATION 3000
#define BUZZER_PIN 12
#define BUZZER_SUCCESS_FREQ 1661
#define BUZZER_FAIL_FREQ 110
#define BUZZER_KEY_FREQ 494

class Buzzer
{
public:
    static void activateSuccessBuzzer()
    {
        tone(BUZZER_PIN, BUZZER_SUCCESS_FREQ, BUZZER_SUCCESS_DURATION);
    }

    static void activateFailBuzzer()
    {
        tone(BUZZER_PIN, BUZZER_FAIL_FREQ, BUZZER_FAIL_DURATION);
    }
};