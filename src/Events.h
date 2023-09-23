#ifndef EVENTS_H
#define EVENTS_H

// EVENTOS
enum Event
{
  Continue,
  ClaveNoConfigurada,
  ClaveConfigurada,
  PersonaDetectadaDia,
  PersonaDetectadaNoche,
  PersonaNoDetectada,
  BotonPresionado,
  ClearClaveIngresada,
  CaracterIngresado,
  ValidarClave,
  TimeOutValidacionClave,
  ClaveInvalida,
  ClaveValida,
  TimeOutAperturaPuerta,
  SeAbrioPuerta,
  SeCerroPuerta,
  NotificarPuertaAbierta
};

const String events_s[] = {
  "Continue",
  "ClaveNoConfigurada",
  "ClaveConfigurada",
  "PersonaDetectadaDia",
  "PersonaDetectadaNoche",
  "PersonaNoDetectada",
  "BotonPresionado",
  "ClearClaveIngresada",
  "CaracterIngresado",
  "ValidarClave",
  "TimeOutValidacionClave",
  "ClaveInvalida",
  "ClaveValida",
  "TimeOutAperturaPuerta",
  "SeAbrioPuerta",
  "SeCerroPuerta",
  "NotificarPuertaAbierta",
};

#define MAX_EVENTS 17

#endif