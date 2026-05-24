/*
 * WiFiConfig - Configuration persistante du module WiFi
 * ------------------------------------------------------
 * ETAPE 1 : Telecharger CE code UNE SEULE FOIS.
 * ETAPE 2 : Verifier dans le moniteur serie (115200) que la connexion reussit.
 * ETAPE 3 : Telecharger ensuite OSX_Projet.ino — le module se connecte seul.
 *
 * Branchement ESP-01 :
 *   ESP-01 RX -> TX1 Mega (pin 18)
 *   ESP-01 TX -> RX1 Mega (pin 19)
 */

#include <WiFiEspAT.h>

const char ssid[] = "Residence-Visiteur";
const char pass[] = "dsfs4Wtr43R";

#define AT_BAUD_RATE 115200

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  while (!Serial);

  Serial1.begin(AT_BAUD_RATE);
  WiFi.init(Serial1);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Module WiFi non detecte!");
    while (true) {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(100);
    }
  }

  WiFi.disconnect();   // Efface l'ancienne connexion
  WiFi.setPersistent(); // La prochaine connexion sera sauvegardee
  WiFi.endAP();        // Desactive le mode point d'acces

  Serial.print("Connexion a : ");
  Serial.println(ssid);

  int status = WiFi.begin(ssid, pass);

  if (status == WL_CONNECTED) {
    Serial.println("Connecte!");
    Serial.print("IP : ");
    Serial.println(WiFi.localIP());
    // LED allumee = succes
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    Serial.println("Echec de connexion.");
    // LED clignote vite = echec
    while (true) {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(100);
    }
  }
}

void loop() {
  // Rien — configuration terminee, telecharger OSX_Projet maintenant
}
