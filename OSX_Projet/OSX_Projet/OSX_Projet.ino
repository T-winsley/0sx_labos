/*
 * OSX_Projet - Projet de session
 * Auteur : Winsley
 *
 * IMPORTANT : Telecharger WiFiConfig.ino UNE FOIS avant ce projet.
 *
 * Brochage :
 *   Bouton urgence   -> pin 6  | Bouton ouverture -> pin 7
 *   LED active       -> pin 2  | LED urgence      -> pin 3
 *   Servo            -> pin 4  | IR               -> pin 12
 *   TRIG             -> pin 10 | ECHO             -> pin 11
 *   Moteur ENA       -> pin 8  | Moteur IN1       -> pin 9
 *   Joystick X       -> A0     | Joystick Y       -> A1
 *   Matrice DIN      -> pin 34 | Matrice CS       -> pin 32
 *   Matrice CLK      -> pin 30
 *   LCD SDA          -> pin 20 | LCD SCL          -> pin 21
 *   ESP-01 RX        -> pin 18 | ESP-01 TX        -> pin 19
 *
 * Moniteur serie : 9600 bauds
 */

#include <OneButton.h>
#include "Porte.h"
#include "Conveyor.h"
#include "SystemeControle.h"
#include "GestionnaireLCD.h"
#include "GestionnaireMQTT.h"

// --- Broches ---
const int BROCHE_BTN_URGENCE = 6;
const int BROCHE_BTN_OUV     = 7;
const int BROCHE_LED_ACTIVE  = 2;
const int BROCHE_LED_URGENCE = 3;
const int BROCHE_SERVO       = 4;
const int BROCHE_IR          = 12;
const int BROCHE_TRIG        = 10;
const int BROCHE_ECHO        = 11;
const int BROCHE_ENA         = 8;
const int BROCHE_IN1         = 9;
const int BROCHE_JOY_X       = A0;
const int BROCHE_JOY_Y       = A1;
const int BROCHE_DIN         = 34;
const int BROCHE_CS          = 32;
const int BROCHE_CLK         = 30;

// --- Modules ---
// Note : Porte recoit -1 pour le bouton car celui-ci est gere par OneButton
Porte            porte(BROCHE_TRIG, BROCHE_ECHO, BROCHE_SERVO, -1);
Conveyor         convoyeur(BROCHE_ENA, BROCHE_IN1,
                           BROCHE_LED_ACTIVE, BROCHE_LED_URGENCE,
                           BROCHE_JOY_X, BROCHE_JOY_Y);
SystemeControle  systeme(BROCHE_CLK, BROCHE_DIN, BROCHE_CS, BROCHE_IR);
GestionnaireLCD  lcd(porte, convoyeur, systeme);
GestionnaireMQTT mqtt;

// --- Boutons OneButton ---
OneButton boutonOuverture(BROCHE_BTN_OUV,     true);
OneButton boutonUrgence  (BROCHE_BTN_URGENCE, true);

bool enUrgence = false;

// Active l'urgence sur tous les modules
void _activerUrgenceGlobale() {
    enUrgence = true;
    porte.activerUrgence();
    convoyeur.activerUrgence();
    systeme.activerUrgence();
    Serial.println("URGENCE activee");
}

// Desactive l'urgence sur tous les modules
void _desactiverUrgenceGlobale() {
    enUrgence = false;
    porte.desactiverUrgence();
    convoyeur.desactiverUrgence();
    systeme.desactiverUrgence();
    Serial.println("URGENCE desactivee");
}

// Bouton 2 : ouverture manuelle de la porte
void surOuverture() {
    if (!enUrgence) {
        porte.ouvrirManuellement();
        Serial.println("Ouverture manuelle");
    }
}

// Bouton 1 : bascule urgence globale (un appui = ON, un autre appui = OFF)
void surUrgence() {
    if (!enUrgence) _activerUrgenceGlobale();
    else            _desactiverUrgenceGlobale();
}

// Retourne la direction du convoyeur pour le payload MQTT
String directionConvoyeur() {
    if      (convoyeur.getState() == CONV_AVANCE) return "droite";
    else if (convoyeur.getState() == CONV_RECULE) return "gauche";
    return "stop";
}

// Retourne l'etat de la porte pour le payload MQTT
String etatPorte() {
    switch (porte.getEtat()) {
        case PORTE_OUVERTURE:
        case PORTE_FERMETURE: return "move";
        case PORTE_OUVERTE:   return "ouvert";
        default:               return "ferme";
    }
}

// Publie l'etat complet sur MQTT toutes les 1000ms
void tachePublierMQTT(unsigned long currentTime) {
    static unsigned long lastPublish = 0;
    if (currentTime - lastPublish < 1000) return;
    lastPublish = currentTime;
    mqtt.publier(directionConvoyeur(), convoyeur.getSpeed(),
                 etatPorte(), systeme.getStatutMQTT(),
                 porte.getNbClients());
}

// Applique les commandes recues du serveur MQTT
// Urgence MQTT active tout le systeme, pas seulement la matrice
void tacheRecevoirMQTT() {
    String statut = mqtt.obtenirStatutRecu();
    if (statut == "") return;

    if (statut.indexOf("urgence") >= 0) {
        _activerUrgenceGlobale();  // Arrete TOUT le systeme
    } else if (statut.indexOf("rabais") >= 0) {
        if (enUrgence) _desactiverUrgenceGlobale();
        SystemeControle::modeActuel = MODE_RABAIS;
    } else if (statut.indexOf("erreur") >= 0) {
        if (enUrgence) _desactiverUrgenceGlobale();
        SystemeControle::modeActuel = MODE_ERREUR;
    } else if (statut.indexOf("ok") >= 0) {
        if (enUrgence) _desactiverUrgenceGlobale();
        SystemeControle::modeActuel = MODE_NORMAL;
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(BROCHE_BTN_OUV,     INPUT_PULLUP);
    pinMode(BROCHE_BTN_URGENCE, INPUT_PULLUP);

    porte.begin();
    convoyeur.begin();
    systeme.initialiser(BROCHE_BTN_URGENCE);
    lcd.begin();
    mqtt.begin();

    boutonOuverture.attachClick(surOuverture);
    boutonUrgence.attachClick(surUrgence);

    Serial.println("Systeme pret.");
}

void loop() {
    unsigned long currentTime = millis();

    // Lecture des boutons (OneButton gere l'anti-rebond)
    boutonOuverture.tick();
    boutonUrgence.tick();

    // Mise a jour des composantes
    porte.update(false);
    convoyeur.update(false);
    systeme.lireEntrees();
    systeme.mettreAJourMatrice(currentTime);

    // LCD cyclique (gele sur URGENCE quand enUrgence)
    lcd.update(enUrgence);

    // MQTT : maintien connexion toutes les 200ms
    static unsigned long lastMqtt = 0;
    if (currentTime - lastMqtt >= 200) {
        lastMqtt = currentTime;
        mqtt.mettreAJour();
    }

    // Publication et reception MQTT
    tachePublierMQTT(currentTime);
    tacheRecevoirMQTT();
}
