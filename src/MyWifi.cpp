#include "MyWifi.h"

#if COMPILAR_PARA_SIMULADOR
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#else
#define WIFI_SSID "Telecentro-08be"
#define WIFI_PASSWORD "4460.1222"
#endif

#define SET_PASS_TOPIC "app-set-pass"
#define COMMAND_TOPIC "app-command"

#define UNLOCK_COMMAND "unlock"

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
}

void MyWifi::Callback(char *topic, byte *message, unsigned int length)
{
    char stMessage[length + 1];
    memcpy(stMessage, message, length);
    stMessage[length] = '\0';
    lastMessage = stMessage;
    lastTopic = topic;
}

void MyWifi::MQTTReconnect()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        if (client.connect(clientId))
        {
            Serial.println("Connected");
            client.subscribe(SET_PASS_TOPIC);
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
    if (lastTopic != "")
    {
        newPassword = lastMessage;
        if (lastTopic == COMMAND_TOPIC && lastMessage == UNLOCK_COMMAND)
        {
            lastMessage = "";
            lastTopic = "";
            event = Event::BotonPresionado;
            return true;
        }
        else if (lastTopic == SET_PASS_TOPIC)
        {
            lastMessage = "";
            lastTopic = "";
            event = Event::PasswordSetted;
            return true;
        }
    }
    return false;
}

String MyWifi::GetNewPassword()
{
    return newPassword;
}