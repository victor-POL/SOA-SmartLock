#include <WiFi.h>

#if COMPILAR_PARA_SIMULADOR
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#else
#define WIFI_SSID "Telecentro-08be"
#define WIFI_PASSWORD "4460.1222"
#endif



class MyWifi
{
private:
public:
    void Setup()
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
};