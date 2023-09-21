#ifndef EVENTS_H
#define EVENTS_H

// EVENTOS
enum Events {
    EVENTO_CONTINUE,
    EVENTO_PERSONA_DETECTADA_DIA,
    EVENTO_PERSONA_DETECTADA_NOCHE,
    EVENTO_PERSONA_NO_DETECTADA,
    EVENTO_VALIDAR_CLAVE,
    EVENTO_CLEAR_CLAVE_INGRESADA,
    EVENTO_CARACTER_INGRESADO,
    EVENTO_TIMEOUT_VALIDACION_CLAVE,
    EVENTO_CLAVE_VALIDA,
    EVENTO_CLAVE_INVALIDA,
    EVENTO_TIMEOUT_APERTURA_PUERTA,
    EVENTO_SE_ABRIO_PUERTA,
    EVENTO_SE_CERRO_PUERTA,
    EVENTO_PUERTA_CERRADA,
    EVENTO_TIMEOUT_CIERRE_PUERTA,
    EVENTO_NOTIFICAR_PUERTA_ABIERTA,
    EVENTO_CLAVE_NO_CONFIGURADA,
    EVENTO_CLAVE_CONFIGURADA,
    EVENTO_BOTON_PRESIONADO
};

#endif