#pragma once

#include <Arduino.h>
#include <LCD_I2C.h>
#include "MatriceDisplay.h"

enum Mode {
    MODE_RABAIS,
    MODE_NORMAL,
    MODE_ERREUR,
    MODE_FERMER,
    MODE_URGENCE
};

class SystemeControle {
public:
    static volatile Mode modeActuel;

    SystemeControle(int clk, int din, int cs, int irPin);

    void initialiser(int boutonPin);
    void lireEntrees();
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
