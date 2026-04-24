#include "Conveyor.h"

Conveyor::Conveyor(int pwm, int dir) {
  _pwm = pwm;
  _dir = dir;
  _etat = STOP;
  _vitesse = 120;
}

void Conveyor::begin() {
  pinMode(_pwm, OUTPUT);
  pinMode(_dir, OUTPUT);
}

void Conveyor::setState(Etat e) {
  _etat = e;
}

void Conveyor::setSpeed(int v) {
  _vitesse = constrain(v, 50, 255);
}

Etat Conveyor::getState() {
  return _etat;
}

int Conveyor::getSpeed() {
  return _vitesse;
}

void Conveyor::stopMotor() {
  analogWrite(_pwm, 0);
  digitalWrite(_dir, LOW);
}

void Conveyor::forward() {
  digitalWrite(_dir, HIGH);
  analogWrite(_pwm, _vitesse);
}

void Conveyor::backward() {
  digitalWrite(_dir, LOW);
  analogWrite(_pwm, _vitesse);
}

void Conveyor::update() {
  switch (_etat) {
    case STOP: stopMotor(); break;
    case AVANCE: forward(); break;
    case RECULE: backward(); break;
    case URGENCE: stopMotor(); break;
  }
}