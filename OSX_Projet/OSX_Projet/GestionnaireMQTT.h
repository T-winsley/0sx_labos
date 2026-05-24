#pragma once
#include <Arduino.h>
#include <WiFiEspAT.h>
#include <PubSubClient.h>

/*
 * GestionnaireMQTT
 * Gere la connexion WiFi persistante et la communication MQTT.
 * Le WiFi doit etre configure UNE FOIS avec WiFiConfig.ino avant usage.
 */
class GestionnaireMQTT {
public:
    GestionnaireMQTT();
    void begin();
    void publier(String direction, int vitesse,
                 String porte, String statut, int clients);
    void mettreAJour();
    String obtenirStatutRecu();

private:
    WiFiClient   _wifiClient;
    PubSubClient _mqttClient;

    const char* _broker   = "216.128.180.194";
    const int   _port     = 1883;
    const char* _mqttUser = "etdshawi";
    const char* _mqttPass = "shawi123";
    const char* _topicPub = "magasin/11/state";
    const char* _topicSub = "magasin/11/set";

    unsigned long _previousMillisWifi = 0;
    unsigned long _previousMillisMQTT = 0;
    const unsigned long DELAI_RECONNEXION = 2000;

    void _connecterWifi();
    void _connecterMQTT();
    static void _gererReception(char* topic, byte* payload, unsigned int length);
};
