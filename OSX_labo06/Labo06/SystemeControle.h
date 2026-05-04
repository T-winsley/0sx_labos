#ifndef SYSTEMECONTROLE_H
#define SYSTEMECONTROLE_H

#include <Arduino.h>
#include <LCD_I2C.h>
#include "MatriceDisplay.h"

enum Mode { RABAIS, NORMAL, ERREUR, FERMER, URGENCE };

class SystemeControle {
private:
    MatriceDisplay matrice;
    LCD_I2C lcd;
    int pinIR;

public:
    static volatile Mode modeActuel;

    SystemeControle(int clk, int din, int cs, int irPin);

    void initialiser(int boutonPin);
    void lireEntrees();
    void mettreAJourAffichage();
    
    static void gestionBouton();
};

#endif