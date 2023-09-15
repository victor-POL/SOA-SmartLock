#include "Ultrasonic.cpp"
#include "Photoresistor.cpp"

#define UMBRAL_LUX_NOCHE 200

class UltrasonicEntrance : public Ultrasonic
{
private:
    Photoresistor photoresistor;

public:
    UltrasonicEntrance(int triggerPin, int echoPin) : Ultrasonic(triggerPin, echoPin), photoresistor(PHOTORESISTOR_PIN)
    {
    }

    bool checkStatus()
    {
        float currentDistance = getDistance();
        float previousDistance = this->previousDistance;
        int luminosity;

        if (currentDistance != previousDistance)
        {
            this->previousDistance = currentDistance;
            luminosity = photoresistor.getLuminosity();

            if (currentDistance > UMBRAL_PERSONA_DETECTADA)
            {
                event = EVENTO_PERSONA_NO_DETECTADA;
            }
            else if (luminosity > UMBRAL_LUX_NOCHE)
            {
                event = EVENTO_PERSONA_DETECTADA_DIA;
            }
            else
            {
                event = EVENTO_PERSONA_DETECTADA_NOCHE;
            }

            return true;
        }

        return false;
    }
};