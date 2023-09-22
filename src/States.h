#ifndef STATES_H
#define STATES_H

// ESTADOS
enum State
{
  CerraduraInit,
  BloqueadoEsperandoVisita,
  EsperandoIngresoClave,
  ValidacionClave,
  EsperandoAperturaPuerta,
  EsperandoAperturaPuertaBoton,
  EsperandoEntradaPersona,
  EsperandoIngresoNuevaClave,
  ConfirmacionNuevaClave,
  ValidacionNuevaClave,
  BloqueadoEsperandoClaveInicial
};

#endif