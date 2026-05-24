#include "GestionnaireMQTT.h"

String statutRecu = "";

GestionnaireMQTT::GestionnaireMQTT()
    : _mqttClient(_wifiClient) {}

void GestionnaireMQTT::begin() {
    Serial1.begin(115200);
    WiFi.init(&Serial1);

    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Module WiFi non detecte!");
        return;
    }

    _connecterWifi();

    _mqttClient.setServer(_broker, _port);
    _mqttClient.setKeepAlive(10);
    _mqttClient.setCallback(_gererReception);

    _connecterMQTT();
}

void GestionnaireMQTT::_connecterWifi() {
    if (WiFi.status() == WL_CONNECTED) return;

    if (millis() - _previousMillisWifi >= DELAI_RECONNEXION) {
        _previousMillisWifi = millis();
        Serial.print("Connexion au WiFi...");

        // Le module se connecte automatiquement au reseau sauvegarde par WiFiConfig.ino
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Connecte!");
            Serial.print("IP : ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("Tentative...");
        }
    }
}

void GestionnaireMQTT::_connecterMQTT() {
    if (_mqttClient.connected()) return;

    if (millis() - _previousMillisMQTT >= DELAI_RECONNEXION) {
        _previousMillisMQTT = millis();
        Serial.print("Connexion au broker MQTT...");

        if (_mqttClient.connect("Winsley_Magasin11", _mqttUser, _mqttPass)) {
            Serial.println("Connecte!");
            _mqttClient.subscribe(_topicSub);
        } else {
            Serial.print("Erreur : ");
            Serial.println(_mqttClient.state());
        }
    }
}

void GestionnaireMQTT::_gererReception(char* topic, byte* payload, unsigned int length) {
    String message = "";
    for (unsigned int i = 0; i < length; i++) message += (char)payload[i];
    statutRecu = message;
    Serial.println("Message recu : " + message);
}

String GestionnaireMQTT::obtenirStatutRecu() {
    String temp = statutRecu;
    statutRecu  = "";
    return temp;
}

void GestionnaireMQTT::publier(String direction, int vitesse,
                                String porte, String statut, int clients) {
    if (!_mqttClient.connected()) return;

    String payload = "{";
    payload += "\"direction\":\"" + direction + "\",";
    payload += "\"vitesse\":"     + String(vitesse) + ",";
    payload += "\"porte\":\""     + porte   + "\",";
    payload += "\"statut\":\""    + statut  + "\",";
    payload += "\"clients\":"     + String(clients);
    payload += "}";

    _mqttClient.publish(_topicPub, payload.c_str());
}

void GestionnaireMQTT::mettreAJour() {
    if (WiFi.status() != WL_CONNECTED) { _connecterWifi();  return; }
    if (!_mqttClient.connected())       { _connecterMQTT(); return; }
    _mqttClient.loop();
}
