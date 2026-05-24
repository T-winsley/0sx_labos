#pragma once

#include <Arduino.h>

enum Etat { CONV_STOP, CONV_AVANCE, CONV_RECULE };

class Conveyor {
public:
    Conveyor(int pwm, int in1,
             int ledActive, int ledUrgence,
             int joyX, int joyY);

    void begin();
    void update(bool urgenceActivee);
    void activerUrgence();
    void desactiverUrgence();

    Etat getState()     const;
    int  getSpeed()     const;
    bool estEnUrgence() const;

private:
    int _pwm, _in1;
    int _ledActive, _ledUrgence;
    int _joyX, _joyY;

    Etat _etat;
    bool _urgence;
    int  _vitesse;

    static const int  VITESSE_MIN          = 0;
    static const int  VITESSE_MAX          = 255;
    static const int  VITESSE_INIT         = 0;
    static const int  SEUIL_HAUT           = 750;
    static const int  SEUIL_BAS            = 250;
    static const int  PAS_VITESSE          = 2;
    static const long DELAI_AJUST_VITES_MS = 150;

    void _appliquerMoteur();
    void _mettreAJourLEDs();
    void _lireJoystick();
    void _stopMotor();
    void _forward();
    void _backward();
};
