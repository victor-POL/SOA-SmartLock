#include <Arduino.h>
#include "States.h"
#include "Events.h"
#include "Connections.h"

extern int event;

class Button
{
    private:

    public:
        Button()
        {
        }

        void setup()
        {
            pinMode(BUTTON_PIN, INPUT);
        }

        bool checkStatus()
        {
            if (digitalRead(BUTTON_PIN) == LOW)
            {
                event = EVENTO_BOTON_PRESIONADO;
                return true;
            }
            return false;
        }
};