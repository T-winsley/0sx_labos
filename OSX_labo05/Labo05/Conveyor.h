#ifndef CONVEYOR_H
#define CONVEYOR_H

#include <Arduino.h>

enum Etat { STOP, AVANCE, RECULE, URGENCE };

class Conveyor {
  public:
    Conveyor(int pwm, int dir);

    void begin();
    void setState(Etat e);
    void setSpeed(int v);

    void update();
    Etat getState();
    int getSpeed();

  private:
    int _pwm;
    int _dir;
    Etat _etat;
    int _vitesse;

    void stopMotor();
    void forward();
    void backward();
};

#endif