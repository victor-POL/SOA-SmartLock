#ifndef STATES_H
#define STATES_H

// ESTADOS
enum State
{
  CerraduraInit,
  BloqueadoEsperandoClaveInicial,
  EsperandoIngresoNuevaClave,
  ConfirmacionNuevaClave,
  ValidacionNuevaClave,
  BloqueadoEsperandoVisita,
  EsperandoIngresoClave,
  ValidacionClave,
  EsperandoAperturaPuerta,
  EsperandoAperturaPuertaBoton,
  EsperandoEntradaPersona,
};

const String states_s[] = {
  "CerraduraInit",
  "BloqueadoEsperandoClaveInicial",
  "EsperandoIngresoNuevaClave",
  "ConfirmacionNuevaClave",
  "ValidacionNuevaClave",
  "BloqueadoEsperandoVisita",
  "EsperandoIngresoClave",
  "ValidacionClave",
  "EsperandoAperturaPuerta",
  "EsperandoAperturaPuertaBoton",
  "EsperandoEntradaPersona",
};

#define MAX_STATES 11

#endif