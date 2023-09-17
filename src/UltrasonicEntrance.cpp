#include "Ultrasonic.cpp"
#include "Photoresistor.cpp"

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
        int currentDistance = getDistance();
        int previousDistance = this->previousDistance;

        if (currentDistance != previousDistance)
        {
            this->previousDistance = currentDistance;
            if (currentDistance > UMBRAL_PERSONA_DETECTADA)
            {
                event = EVENTO_PERSONA_NO_DETECTADA;
                return true;
            }
            else
            {
                event = photoresistor.getStatus() == "DAY" ? EVENTO_PERSONA_DETECTADA_DIA : EVENTO_PERSONA_DETECTADA_NOCHE;
                return true;
            }
        }
        return false;
    }
};