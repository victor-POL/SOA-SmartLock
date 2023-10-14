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
  NotificarPuertaAbierta,
  AperturaNFC,
  NFCIncorrecto,
  MensajeRecibido
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
  "AperturaNFC",
  "NFCIncorrecto",
  "MensajeRecibido"
  };

#define MAX_EVENTS 20

#endif