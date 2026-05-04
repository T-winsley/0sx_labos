#include "SystemeControle.h"   // ← plus besoin de IRremote.hpp ici

SystemeControle monSysteme(6, 4, 5, 11);

void setup() {
    monSysteme.initialiser(3);
}

void loop() {
    monSysteme.lireEntrees();
    monSysteme.mettreAJourAffichage();
    delay(50);
}