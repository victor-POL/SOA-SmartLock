#ifndef MYWIFI_H
#define MYWIFI_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "Config.h"
#include "Events.h"

extern enum Event event;

class MyWifi
{
private:
public:
    static char clientId[50];
    static WiFiClient espClient;
    static PubSubClient client; // No la inicialices aquí
    void SetupWifi();
    static void Callback(char* topic, byte* message, unsigned int length);
    void MQTTReconnect();
    void SetupMQTT();

    void SendData(String topic, String message);
    void CheckMQTT();
    bool CheckLastMessage();
};

#endif
