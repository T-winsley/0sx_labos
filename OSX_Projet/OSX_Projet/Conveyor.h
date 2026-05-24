#pragma once

#include <Arduino.h>

/*
 * Etat
 * ----
 * Machine a etats du convoyeur.
 * Prefixe CONV_ pour eviter les conflits avec d'autres enums.
 */
enum Etat { CONV_STOP, CONV_AVANCE, CONV_RECULE };

/*
 * Conveyor
 * --------
 * Gere le convoyeur de la caisse avec moteur DC (L293D), joystick et LEDs.
 * Axe Y : haut = avance, bas = recule
 * Axe X : droite = accelere, gauche = ralentit
 */
class Conveyor {
public:
    // pwm : ENA (pin 44) | in1 : IN1 (pin 45) | IN2 cable a GND
    Conveyor(int pwm, int in1,
             int ledActive, int ledUrgence,
             int joyX, int joyY);

    void begin();
    // urgenceActivee : true si le bouton urgence global vient d'etre presse
    void update(bool urgenceActivee);

    void activerUrgence();
    void desactiverUrgence();

    Etat getState()     const;
    int  getSpeed()     const;
    bool estEnUrgence() const;

private:
    int _pwm;
    int _in1;
    // IN2 est cable a GND physiquement — pas de broche logicielle
    int _ledActive;
    int _ledUrgence;
    int _joyX;
    int _joyY;

    Etat _etat;
    bool _urgence;
    int  _vitesse;

    static const int  VITESSE_MIN          = 0;
    static const int  VITESSE_MAX          = 255;
    static const int  VITESSE_INIT         = 0;  // était 0
    static const int  SEUIL_HAUT           = 750;  // comme ton labo
    static const int  SEUIL_BAS            = 250;  // comme ton labo
    static const int  PAS_VITESSE          = 2;    // était 1
    static const long DELAI_AJUST_VITES_MS = 150;  // était 50

    void _appliquerMoteur();
    void _mettreAJourLEDs();
    void _lireJoystick();
    void _stopMotor();
    void _forward();
    void _backward();
};
