#pragma once

#include <Arduino.h>
#include <LCD_I2C.h>
#include "MatriceDisplay.h"

/*
 * Mode
 * ----
 * Etats d'affichage de la matrice LED.
 * Prefixe MODE_ pour eviter les conflits avec enum Etat du convoyeur.
 */
enum Mode {
    MODE_RABAIS,   // Defiler "SPECIAL"
    MODE_NORMAL,   // Afficher smiley
    MODE_ERREUR,   // Clignoter toutes les LEDs
    MODE_FERMER,   // Eteindre la matrice
    MODE_URGENCE   // Afficher X
};

/*
 * SystemeControle
 * ---------------
 * Gere la matrice LED : lecture des commandes IR et serie,
 * mise a jour de l'affichage et gestion du mode urgence.
 */
class SystemeControle {
public:
    // modeActuel : volatile car potentiellement modifie par interruption
    static volatile Mode modeActuel;

    SystemeControle(int clk, int din, int cs, int irPin);

    void initialiser(int boutonPin);

    // Lit les commandes serie (1/2/3/4) et IR pour changer le mode
    void lireEntrees();

    // Met a jour l'affichage de la matrice (prend le temps courant pour les animations)
    void mettreAJourMatrice(unsigned long currentTime);

    void activerUrgence();
    void desactiverUrgence();

    Mode        getMode()       const;
    const char* getNomMode()    const;
    const char* getStatutMQTT() const;

    static void gestionBouton();

private:
    MatriceDisplay _matrice;
    int            _pinIR;
};
