#include <OneButton.h>
#include "Porte.h"
#include "Conveyor.h"
#include "SystemeControle.h"
#include "GestionnaireLCD.h"
#include "GestionnaireMQTT.h"

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

Porte            porte(BROCHE_TRIG, BROCHE_ECHO, BROCHE_SERVO, -1);
Conveyor         convoyeur(BROCHE_ENA, BROCHE_IN1,
                           BROCHE_LED_ACTIVE, BROCHE_LED_URGENCE,
                           BROCHE_JOY_X, BROCHE_JOY_Y);
SystemeControle  systeme(BROCHE_CLK, BROCHE_DIN, BROCHE_CS, BROCHE_IR);
GestionnaireLCD  lcd(porte, convoyeur, systeme);
GestionnaireMQTT mqtt;

OneButton boutonOuverture(BROCHE_BTN_OUV,     true);
OneButton boutonUrgence  (BROCHE_BTN_URGENCE, true);

bool enUrgence = false;

void _activerUrgenceGlobale() {
    enUrgence = true;
    porte.activerUrgence();
    convoyeur.activerUrgence();
    systeme.activerUrgence();
    Serial.println("URGENCE activee");
}

void _desactiverUrgenceGlobale() {
    enUrgence = false;
    porte.desactiverUrgence();
    convoyeur.desactiverUrgence();
    systeme.desactiverUrgence();
    Serial.println("URGENCE desactivee");
}

void surOuverture() {
    if (!enUrgence) {
        porte.ouvrirManuellement();
        Serial.println("Ouverture manuelle");
    }
}

void surUrgence() {
    if (!enUrgence) _activerUrgenceGlobale();
    else            _desactiverUrgenceGlobale();
}

String directionConvoyeur() {
    if      (convoyeur.getState() == CONV_AVANCE) return "droite";
    else if (convoyeur.getState() == CONV_RECULE) return "gauche";
    return "stop";
}

String etatPorte() {
    switch (porte.getEtat()) {
        case PORTE_OUVERTURE:
        case PORTE_FERMETURE: return "move";
        case PORTE_OUVERTE:   return "ouvert";
        default:              return "ferme";
    }
}

void tachePublierMQTT(unsigned long currentTime) {
    static unsigned long lastPublish = 0;
    if (currentTime - lastPublish < 1000) return;
    lastPublish = currentTime;
    mqtt.publier(directionConvoyeur(), convoyeur.getSpeed(),
                 etatPorte(), systeme.getStatutMQTT(),
                 porte.getNbClients());
}

void tacheRecevoirMQTT() {
    String statut = mqtt.obtenirStatutRecu();
    if (statut == "") return;

    if      (statut.indexOf("urgence") >= 0) {
        _activerUrgenceGlobale();
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
    pinMode(LED_BUILTIN,        OUTPUT);
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

    boutonOuverture.tick();
    boutonUrgence.tick();

    porte.update(false);
    convoyeur.update(false);
    systeme.lireEntrees();
    systeme.mettreAJourMatrice(currentTime);

    lcd.update(enUrgence);

    static unsigned long lastMqtt = 0;
    if (currentTime - lastMqtt >= 200) {
        lastMqtt = currentTime;
        mqtt.mettreAJour();
    }

    tachePublierMQTT(currentTime);
    tacheRecevoirMQTT();
}
