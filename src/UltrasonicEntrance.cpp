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

  bool CheckStatus()
  {
    int currentDistance = GetDistance();
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
        event = photoresistor.GetStatus() == "DAY" ? EVENTO_PERSONA_DETECTADA_DIA : EVENTO_PERSONA_DETECTADA_NOCHE;
        return true;
      }
    }
    return false;
  }
};