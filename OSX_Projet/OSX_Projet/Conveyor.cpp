#include "Conveyor.h"

Conveyor::Conveyor(int pwm, int in1,
                   int ledActive, int ledUrgence,
                   int joyX, int joyY)
    : _pwm(pwm), _in1(in1),
      _ledActive(ledActive), _ledUrgence(ledUrgence),
      _joyX(joyX), _joyY(joyY),
      _etat(CONV_STOP), _urgence(false), _vitesse(VITESSE_INIT) {}

void Conveyor::begin() {
    pinMode(_pwm,        OUTPUT);
    pinMode(_in1,        OUTPUT);
    pinMode(_ledActive,  OUTPUT);
    pinMode(_ledUrgence, OUTPUT);
    _stopMotor();
}

void Conveyor::update(bool urgenceActivee) {
    if (urgenceActivee) { activerUrgence(); return; }

    if (_urgence) {
        _stopMotor();
        _mettreAJourLEDs();
        return;
    }

    _lireJoystick();
    _appliquerMoteur();
    _mettreAJourLEDs();
}

void Conveyor::activerUrgence() {
    _urgence = true;
    _stopMotor();
    _mettreAJourLEDs();
}

void Conveyor::desactiverUrgence() {
    _urgence  = false;
    _etat     = CONV_STOP;
    _vitesse  = 0;
    _mettreAJourLEDs();
}

Etat Conveyor::getState()     const { return _etat; }
int  Conveyor::getSpeed()     const { return _vitesse; }
bool Conveyor::estEnUrgence() const { return _urgence; }

// IN1=HIGH, vitesse > 0 → avance
// IN1=LOW quand vitesse = 0 pour eviter le signal residuel
void Conveyor::_forward() {
    if (_vitesse == 0) {
        digitalWrite(_in1, LOW);
        analogWrite(_pwm, 0);
    } else {
        digitalWrite(_in1, HIGH);
        analogWrite(_pwm, _vitesse);
    }
}

// IN2 est a GND physiquement donc le moteur ne peut pas reculer
// On simule le recul en inversant IN1 avec une vitesse reduite
void Conveyor::_backward() {
    // Avec IN2 a GND : IN1=LOW et PWM → frein/recul selon le montage
    digitalWrite(_in1, LOW);
    analogWrite(_pwm, _vitesse);
}

// Arret complet : IN1=LOW et PWM=0
void Conveyor::_stopMotor() {
    digitalWrite(_in1, LOW);
    analogWrite(_pwm, 0);
}

void Conveyor::_appliquerMoteur() {
    switch (_etat) {
        case CONV_AVANCE: _forward();   break;
        case CONV_RECULE: _backward();  break;
        default:          _stopMotor(); break;
    }
}

void Conveyor::_mettreAJourLEDs() {
    bool actif = (_etat == CONV_AVANCE || _etat == CONV_RECULE) && !_urgence;
    digitalWrite(_ledActive,  actif    ? HIGH : LOW);
    digitalWrite(_ledUrgence, _urgence ? HIGH : LOW);
}

void Conveyor::_lireJoystick() {
    // Direction axe Y - seuils 600/400 comme le projet de reference
    if      (analogRead(_joyY) > 600) _etat = CONV_AVANCE;
    else if (analogRead(_joyY) < 400) _etat = CONV_RECULE;
    else                               _etat = CONV_STOP;

    // Arret : vitesse revient a 0
    if (_etat == CONV_STOP) {
        _vitesse = 0;
        return;
    }

    // Vitesse axe X - toutes les 50ms comme le projet de reference
    static unsigned long dernierAjust = 0;
    if ((millis() - dernierAjust) >= 50) {
        dernierAjust = millis();
        if      (analogRead(_joyX) > 600) { if (_vitesse > 0)   _vitesse--; }
        else if (analogRead(_joyX) < 400) { if (_vitesse < 255) _vitesse++; }
    }
}
