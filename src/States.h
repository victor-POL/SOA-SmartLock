#ifndef STATES_H
#define STATES_H

// ESTADOS
enum States {
    ESTADO_CERRADURA_INIT,
    ESTADO_BLOQUEADO_ESPERANDO_VISITA,
    ESTADO_ESPERANDO_INGRESO_CONTRASENA,
    ESTADO_VALIDACION_CLAVE,
    ESTADO_ESPERANDO_APERTURA_PUERTA,
    ESTADO_ESPERANDO_APERTURA_PUERTA_BOTON,
    ESTADO_ESPERANDO_CIERRE_PUERTA,
    ESTADO_ESPERANDO_ENTRADA_PERSONA,
    ESTADO_ESPERANDO_INGRESO_NUEVA_CLAVE,
    ESTADO_CONFIRMACION_NUEVA_CLAVE,
    ESTADO_VALIDACION_NUEVA_CLAVE,
    ESTADO_BLOQUEADO_ESPERANDO_CLAVE_INICIAL
};

#endif