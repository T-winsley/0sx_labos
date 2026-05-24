#include "SystemeControle.h"

// IRremote inclus ici seulement (evite les conflits de symboles)
#define NO_LED_FEEDBACK_CODE
#define EXCLUDE_EXOTIC_PROTOCOLS
#include <IRremote.hpp>

volatile Mode SystemeControle::modeActuel = MODE_NORMAL;

SystemeControle::SystemeControle(int clk, int din, int cs, int irPin)
    : _matrice(clk, din, cs), _pinIR(irPin) {}

void SystemeControle::initialiser(int boutonPin) {
    _matrice.begin();
    IrReceiver.begin(_pinIR);
    pinMode(boutonPin, INPUT_PULLUP);
}

// Lit les commandes du port serie et de la telecommande IR.
// Les commandes sont ignorees si le systeme est en mode urgence.
void SystemeControle::lireEntrees() {
    if (modeActuel == MODE_URGENCE) return;

    // Commandes serie : 1=RABAIS, 2=NORMAL, 3=ERREUR, 4=FERMER
    if (Serial.available()) {
        char c = Serial.read();
        if      (c == '1') modeActuel = MODE_RABAIS;
        else if (c == '2') modeActuel = MODE_NORMAL;
        else if (c == '3') modeActuel = MODE_ERREUR;
        else if (c == '4') modeActuel = MODE_FERMER;
    }

    // Commandes IR (codes hexadecimaux de la telecommande fournie)
    if (IrReceiver.decode()) {
        uint32_t val = IrReceiver.decodedIRData.decodedRawData;
        if (val != 0) {
            if      (val == 0xF30CFF00) modeActuel = MODE_RABAIS;
            else if (val == 0xE718FF00) modeActuel = MODE_NORMAL;
            else if (val == 0xA15EFF00) modeActuel = MODE_ERREUR;
            else if (val == 0xF708FF00) modeActuel = MODE_FERMER;
        }
        IrReceiver.resume();
    }
}

// Met a jour la matrice en fonction du mode actuel.
// currentTime est passe aux animations non-bloquantes (rabais et erreur).
void SystemeControle::mettreAJourMatrice(unsigned long currentTime) {
    switch (modeActuel) {
        case MODE_RABAIS:  _matrice.afficherRabais(currentTime);  break;
        case MODE_NORMAL:  _matrice.afficherNormal();              break;
        case MODE_ERREUR:  _matrice.afficherErreur(currentTime);  break;
        case MODE_FERMER:  _matrice.eteindre();                    break;
        case MODE_URGENCE: _matrice.afficherUrgence();             break;
    }
}

void SystemeControle::activerUrgence()    { modeActuel = MODE_URGENCE; }
void SystemeControle::desactiverUrgence() { modeActuel = MODE_NORMAL; }

Mode SystemeControle::getMode() const { return modeActuel; }

// Retourne le nom du mode pour l'affichage sur le LCD
const char* SystemeControle::getNomMode() const {
    switch (modeActuel) {
        case MODE_RABAIS:  return "RABAIS";
        case MODE_NORMAL:  return "NORMAL";
        case MODE_ERREUR:  return "ERREUR";
        case MODE_FERMER:  return "FERMER";
        case MODE_URGENCE: return "URGENCE";
        default:           return "INCONNU";
    }
}

// Retourne le statut pour le payload MQTT
const char* SystemeControle::getStatutMQTT() const {
    switch (modeActuel) {
        case MODE_RABAIS:  return "rabais";
        case MODE_ERREUR:  return "erreur";
        case MODE_URGENCE: return "urgence";
        default:           return "ok";
    }
}

// Interruption : bascule urgence via bouton (garde pour compatibilite)
void SystemeControle::gestionBouton() {
    static unsigned long dernierTemps = 0;
    unsigned long tempsActuel = millis();
    if (tempsActuel - dernierTemps > 250) {
        modeActuel   = (modeActuel == MODE_URGENCE) ? MODE_NORMAL : MODE_URGENCE;
        dernierTemps = tempsActuel;
    }
}
