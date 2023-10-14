#include "MyWifi.h"

#if COMPILAR_PARA_SIMULADOR
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#else
#define WIFI_SSID "Telecentro-08be"
#define WIFI_PASSWORD "4460.1222"
#endif

#define UNLOCK_TOPIC "esp-unlock"
#define COMMAND_TOPIC "esp-command"

#define BROKER_URL "broker.emqx.io"
#define BROKER_PORT 1883

char MyWifi::clientId[50];
WiFiClient MyWifi::espClient;
PubSubClient MyWifi::client(MyWifi::espClient);
String lastMessage = "";
String lastTopic = "";

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
}

void MyWifi::Callback(char *topic, byte *message, unsigned int length)
{
    char stMessage[length + 1];
    memcpy(stMessage, message, length);
    stMessage[length] = '\0';
    lastMessage = stMessage;
    lastTopic = topic;
    Serial.println(lastTopic);
    Serial.println(lastMessage);
}

void MyWifi::MQTTReconnect()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        if (client.connect(clientId))
        {
            Serial.println("Connected");
            client.subscribe(UNLOCK_TOPIC);
            client.subscribe(COMMAND_TOPIC);
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
    client.setServer(BROKER_URL, BROKER_PORT);
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

bool MyWifi::CheckLastMessage()
{
    if (lastMessage != "")
    {
        Serial.println("Tópico recibido: " + lastTopic);
        Serial.println("Mensaje recibido: " + lastMessage);
        lastMessage = "";
        lastTopic = "";

        event = Event::MensajeRecibido;
        return true;
    }
    return false;
}