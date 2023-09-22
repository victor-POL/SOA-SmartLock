#ifndef EVENTS_H
#define EVENTS_H

// EVENTOS
enum Event
{
  Continue,
  PersonaDetectadaDia,
  PersonaDetectadaNoche,
  PersonaNoDetectada,
  ValidarClave,
  ClearClaveIngresada,
  CaracterIngresado,
  TimeOutValidacionClave,
  ClaveValida,
  ClaveInvalida,
  TimeOutAperturaPuerta,
  SeAbrioPuerta,
  SeCerroPuerta,
  NotificarPuertaAbierta,
  ClaveNoConfigurada,
  ClaveConfigurada,
  BotonPresionado
};

#endif