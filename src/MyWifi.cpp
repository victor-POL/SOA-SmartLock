#include "MyWifi.h"

#if COMPILAR_PARA_SIMULADOR
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#else
#define WIFI_SSID "Telecentro-08be"
#define WIFI_PASSWORD "4460.1222"
#endif

char MyWifi::clientId[50];
WiFiClient MyWifi::espClient;
PubSubClient MyWifi::client(MyWifi::espClient); // Inicializa la variable aquí

void MyWifi::SetupWifi()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
}

void MyWifi::Callback(char *topic, byte *message, unsigned int length)
{
    String stMessage;
    for (int i = 0; i < length; i++)
    {
        stMessage += (char)message[i];
    }
    Serial.println(stMessage);
}

void MyWifi::MQTTReconnect()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        if (client.connect(clientId))
        {
            Serial.println("Connected");
            client.subscribe("aplicacion");
        }
        else
        {
            Serial.println("Connection failed, try again in 5 seconds");
            delay(5000);
        }
    }
}

void MyWifi::SetupMQTT()
{
    client.setServer("broker.emqx.io", 1883);
    client.setCallback(MyWifi::Callback);
    MQTTReconnect();
}

void MyWifi::SendData(String topic, String message)
{
    client.publish(topic.c_str(), message.c_str());
}

void MyWifi::CheckMQTT()
{
    if (!client.connected())
    {
        MQTTReconnect();
    }
    client.loop();
}