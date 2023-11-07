#ifndef EVENTS_H
#define EVENTS_H

enum Event
{
  Continue,
  ClaveNoConfigurada,
  ClaveConfigurada,
  PersonaDetectadaDia,
  PersonaDetectadaNoche,
  PersonaNoDetectada,
  DesbloqueoManual,
  ClearClaveIngresada,
  CaracterIngresado,
  ValidarClave,
  TimeOutValidacionClave,
  ClaveInvalida,
  ClaveValida,
  TimeOutAperturaPuerta,
  SeAbrioPuerta,
  SeCerroPuerta,
  NotificarPuertaAbierta,
  NFCIncorrecto,
  PasswordSetted
};

const String events_s[] = {
  "Continue",
  "ClaveNoConfigurada",
  "ClaveConfigurada",
  "PersonaDetectadaDia",
  "PersonaDetectadaNoche",
  "PersonaNoDetectada",
  "DesbloqueoManual",
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
  "NFCIncorrecto",
  "PasswordSetted"
  };

#define MAX_EVENTS 19

#endif