#include <Arduino.h>
#include <IRremote.hpp>
#include "SystemeControle.h"

// ... reste identique
// Initialisation de la variable statique
volatile Mode SystemeControle::modeActuel = NORMAL;

SystemeControle::SystemeControle(int clk, int din, int cs, int irPin) 
    : matrice(clk, din, cs), lcd(0x27, 16, 2), pinIR(irPin) {}

void SystemeControle::initialiser(int boutonPin) {
    Serial.begin(9600);
    matrice.begin();
    lcd.begin();
    lcd.backlight();
    IrReceiver.begin(pinIR);
    pinMode(boutonPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(boutonPin), gestionBouton, FALLING);
}

void SystemeControle::gestionBouton() {
    static unsigned long dernierTemps = 0;
    unsigned long tempsActuel = millis();    
    if (tempsActuel - dernierTemps > 250) {
        if (modeActuel == URGENCE) {
            modeActuel = NORMAL;
        } else {
            modeActuel = URGENCE;
        }
        dernierTemps = tempsActuel;
    }
}

void SystemeControle::lireEntrees() {
    if (modeActuel != URGENCE) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '1') { modeActuel = RABAIS; }
            if (c == '2') { modeActuel = NORMAL; }
            if (c == '3') { modeActuel = ERREUR; }
            if (c == '4') { modeActuel = FERMER; }
        }
        if (IrReceiver.decode()) {
            uint32_t val = IrReceiver.decodedIRData.decodedRawData;
            if (val != 0) { 
                if (val == 0xF30CFF00)      { modeActuel = RABAIS; }
                else if (val == 0xE718FF00) { modeActuel = NORMAL; }
                else if (val == 0xA15EFF00) { modeActuel = ERREUR; }
                else if (val == 0xF708FF00) { modeActuel = FERMER; }
            }
            IrReceiver.resume(); 
        }
    }
}

void SystemeControle::mettreAJourAffichage() {
    lcd.setCursor(0, 0);
    switch (modeActuel) {
        case RABAIS:
            lcd.print("MODE: RABAIS   ");
            matrice.afficherRabais();
            break;
        case NORMAL:
            lcd.print("MODE: NORMAL   ");
            matrice.afficherNormal();
            break;
        case ERREUR:
            lcd.print("MODE: ERREUR   ");
            matrice.afficherErreur();
            break;
        case FERMER:
            lcd.print("MODE: FERMER   ");
            matrice.eteindre();
            break;
        case URGENCE:
            lcd.setCursor(0, 0);
            lcd.print("!! URGENCE !!  ");
            matrice.afficherUrgence();
            break;
    }
}