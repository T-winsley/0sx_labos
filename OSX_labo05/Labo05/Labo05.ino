#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Conveyor.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

Conveyor conveyor(44, 45);

// PINS
const int LED1 = 6;
const int LED2 = 5;
const int BOUTON = 2;
const int JOY_X = A0;
const int JOY_Y = A1;

// LCD
unsigned long lastLCD = 0;
int page = 0;

// bouton
bool lastButton = HIGH;
unsigned long lastDebounce = 0;

// JOYSTICK
int readX() { return analogRead(JOY_X); }
int readY() { return analogRead(JOY_Y); }

//  SETUP 
void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BOUTON, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  conveyor.begin();
}

//  LOGIQUE 
void handleButton() {
  bool reading = digitalRead(BOUTON);

  if (reading == LOW && lastButton == HIGH && millis() - lastDebounce > 200) {
    if (conveyor.getState() != URGENCE)
      conveyor.setState(URGENCE);
    else
      conveyor.setState(STOP);

    lastDebounce = millis();
  }

  lastButton = reading;
}

void handleJoystick() {
  int y = readY();

  if (y > 750) conveyor.setState(AVANCE);
  else if (y < 250) conveyor.setState(RECULE);
  else conveyor.setState(STOP);

  static unsigned long lastSpeed = 0;
  int x = readX();

  if (millis() - lastSpeed > 150) {
    if (x > 750) conveyor.setSpeed(conveyor.getSpeed() + 2);
    else if (x < 250) conveyor.setSpeed(conveyor.getSpeed() - 2);

    lastSpeed = millis();
  }
}

void handleLEDs() {
  digitalWrite(LED1, conveyor.getState() != STOP);
  digitalWrite(LED2, conveyor.getState() == URGENCE);
}

void handleLCD() {
  if (millis() - lastLCD > 2000) {
    lcd.clear();

    if (page == 0) {
      lcd.print("Convoyeur:");
      lcd.setCursor(0,1);
      lcd.print(conveyor.getState() == STOP ? "NON" : "OUI");
    }
    else if (page == 1) {
      lcd.print("Sens:");
      lcd.setCursor(0,1);
      if (conveyor.getState() == AVANCE) lcd.print("AVANT");
      else if (conveyor.getState() == RECULE) lcd.print("ARRIERE");
      else lcd.print("-");
    }
    else {
      lcd.print("Vitesse:");
      lcd.setCursor(0,1);
      lcd.print(conveyor.getSpeed());
    }

    page = (page + 1) % 3;
    lastLCD = millis();
  }
}

//  LOOP 
void loop() {

  handleButton();

  if (conveyor.getState() == URGENCE) {
    conveyor.update();

    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("URGENCE");
    lcd.setCursor(0,1);
    lcd.print("SYSTEM STOP");

    return;
  }

  handleJoystick();
  handleLEDs();
  conveyor.update();
  handleLCD();
}