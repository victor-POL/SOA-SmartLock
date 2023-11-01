#include "Ultrasonic.cpp"
#include "Photoresistor.cpp"

#define UMBRAL_CAMBIO_VALOR_PERSONA 3
#define UMBRAL_PERSONA_DETECTADA 20

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
    
    if (abs(current_distance - previous_distance) > UMBRAL_CAMBIO_VALOR_PERSONA)
    {
      this->previous_distance = current_distance;
      if (current_distance > UMBRAL_PERSONA_DETECTADA)
      {
        event = Event::PersonaNoDetectada;
        return true;
      }
      else
      {
        event = photoresistor.GetStatus() == "DAY" ? Event::PersonaDetectadaDia : Event::PersonaDetectadaNoche;
        return true;
      }
    }
    return false;
  }
};