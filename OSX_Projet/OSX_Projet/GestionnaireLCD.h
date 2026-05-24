#pragma once

#include <Arduino.h>
#include <LCD_I2C.h>
#include "Porte.h"
#include "Conveyor.h"
#include "SystemeControle.h"

class GestionnaireLCD {
public:
    GestionnaireLCD(Porte& porte, Conveyor& convoyeur, SystemeControle& systeme);

    void begin();

    // Appeler a chaque iteration de loop().
    // En urgence : affiche uniquement "URGENCE", arrete le defilement.
    // Sinon : defilement cyclique entre 3 pages (porte / convoyeur / matrice).
    void update(bool enUrgence);

private:
    LCD_I2C _lcd;
    Porte&            _porte;
    Conveyor&         _convoyeur;
    SystemeControle&  _systeme;

    int           _pageCourante;
    unsigned long _dernierChangPage;
    bool          _urgenceAffichee;

    static const long DELAI_PAGE_MS = 3000;

    void _afficherPagePorte();
    void _afficherPageConvoyeur();
    void _afficherPageMatrice();
    void _afficherPageUrgence();
};
