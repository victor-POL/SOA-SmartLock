#include "Ultrasonic.cpp"
#include "Photoresistor.cpp"

class UltrasonicEntrance : public Ultrasonic
{
private:
  Photoresistor photoresistor;

public:
  UltrasonicEntrance(int trigger_pin, int echo_pin) : Ultrasonic(trigger_pin, echo_pin), photoresistor(PHOTORESISTOR_PIN)
  {
  }

  bool CheckStatus()
  {
    int current_distance = GetDistance();
    int previous_distance = this->previous_distance;

    if (current_distance != previous_distance)
    {
      this->previous_distance = current_distance;
      if (current_distance > UMBRAL_PERSONA_DETECTADA)
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