#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Broches
const int BROCHE_SERVO         = 7;
const int BROCHE_TRIG          = 9;
const int BROCHE_ECHO          = 10;
const int BROCHE_BTN_OUVERTURE = 2;
const int BROCHE_BTN_URGENCE   = 3;

//Constantes de configuration
const int  DISTANCE_DETECTION_CM = 10;
const int  ANGLE_FERME           = 10;
const int  ANGLE_OUVERT          = 170;
const int  DELAI_ATTENTE_MS      = 10000;
const int  DELAI_MOUVEMENT_MS    = 15;
const long DELAI_ANTI_REBOND_MS  = 50;

//États de la machine
enum EtatPorte {
  ETAT_FERME,
  ETAT_OUVERTURE,
  ETAT_OUVERT,
  ETAT_FERMETURE,
  ETAT_URGENCE
};

//Variables globales
EtatPorte etatActuel    = ETAT_FERME;
int       angleActuel   = ANGLE_FERME;
long      tempsDebutAttente = 0;
long      dernierMouvServo  = 0;

//Anti-rebond boutons
bool btnOuvertureEtatPrecedent = HIGH;
bool btnUrgenceEtatPrecedent   = HIGH;
long dernierChangBtnOuv        = 0;
long dernierChangBtnUrg        = 0;

//Indicateur de rafraîchissement LCD
EtatPorte dernierEtatAffiche = (EtatPorte)-1;

Servo             servo;
LiquidCrystal_I2C lcd(0x27, 16, 2);


//Mesure la distance en cm via le HC-SR04. Retourne -1 si hors de portée.
int lireDistance() {
  digitalWrite(BROCHE_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(BROCHE_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(BROCHE_TRIG, LOW);

  long duree = pulseIn(BROCHE_ECHO, HIGH, 30000);
  if (duree == 0) return -1;
  return (int)(duree * 0.034 / 2);
}

//Lit un bouton avec anti-rebond. Retourne true UNIQUEMENT sur le front descendant (une seule fois par appui).
bool boutonAppuye(int broche, bool &etatPrecedent, long &dernierChangement) {
  bool etatCourant = digitalRead(broche);

  if (etatCourant != etatPrecedent) {
    dernierChangement = millis();
    etatPrecedent = etatCourant;
  }

  if ((millis() - dernierChangement) > DELAI_ANTI_REBOND_MS && etatCourant == LOW) {
    etatPrecedent = HIGH; // Consomme l'événement
    return true;
  }
  return false;
}

//Met à jour l'affichage LCD uniquement si l'état a changé.
void mettreAJourLCD(EtatPorte etat) {
  if (etat == dernierEtatAffiche) return;
  dernierEtatAffiche = etat;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Porte:");

  switch (etat) {
    case ETAT_FERME:
      lcd.setCursor(0, 1);
      lcd.print("Fermee          ");
      break;
    case ETAT_OUVERTURE:
      lcd.setCursor(0, 1);
      lcd.print("En ouverture... ");
      break;
    case ETAT_OUVERT:
      lcd.setCursor(0, 1);
      lcd.print("Ouverte         ");
      break;
    case ETAT_FERMETURE:
      lcd.setCursor(0, 1);
      lcd.print("En fermeture... ");
      break;
    case ETAT_URGENCE:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("!! URGENCE !!   ");
      lcd.setCursor(0, 1);
      lcd.print("Systeme arrete  ");
      break;
  }
}

//Déplace le servo d'un degré vers l'angle cible de façon non-bloquante. Retourne true quand l'angle cible est atteint.
bool deplacerServoVers(int angleCible) {
  long maintenant = millis();
  if (maintenant - dernierMouvServo < DELAI_MOUVEMENT_MS) return false;
  dernierMouvServo = maintenant;

  if (angleActuel < angleCible) {
    angleActuel++;
  } else if (angleActuel > angleCible) {
    angleActuel--;
  }

  servo.write(angleActuel);
  return (angleActuel == angleCible);
}

//Gère la transition vers un nouvel état.
void entrerEtat(EtatPorte nouvelEtat) {
  etatActuel = nouvelEtat;
  mettreAJourLCD(etatActuel);

  if (nouvelEtat == ETAT_OUVERT) {
    tempsDebutAttente = millis();
  }
}


void setup() {
  pinMode(BROCHE_TRIG, OUTPUT);
  pinMode(BROCHE_ECHO, INPUT);
  pinMode(BROCHE_BTN_OUVERTURE, INPUT_PULLUP);
  pinMode(BROCHE_BTN_URGENCE,   INPUT_PULLUP);

  servo.attach(BROCHE_SERVO);
  servo.write(ANGLE_FERME);
  angleActuel = ANGLE_FERME;

  lcd.init();
  lcd.backlight();

  entrerEtat(ETAT_FERME);

  Serial.begin(9600);
  Serial.println("Systeme demarre - ETAT_FERME");
}


void loop() {
  //Lecture boutons (anti-rebond)
  bool btnOuvAppuye = boutonAppuye(
    BROCHE_BTN_OUVERTURE,
    btnOuvertureEtatPrecedent,
    dernierChangBtnOuv
  );

  bool btnUrgAppuye = boutonAppuye(
    BROCHE_BTN_URGENCE,
    btnUrgenceEtatPrecedent,
    dernierChangBtnUrg
  );

  //Machine à états
  switch (etatActuel) {

    case ETAT_FERME: {
      int distance = lireDistance();
      bool clientDetecte = (distance > 0 && distance <= DISTANCE_DETECTION_CM);

      if (btnUrgAppuye) {
        entrerEtat(ETAT_URGENCE);
        Serial.println("Urgence activee depuis FERME");
      } else if (clientDetecte || btnOuvAppuye) {
        entrerEtat(ETAT_OUVERTURE);
        Serial.println("Debut ouverture");
      }
      break;
    }

    case ETAT_OUVERTURE: {
      if (btnUrgAppuye) {
        entrerEtat(ETAT_URGENCE);
        Serial.println("Urgence activee durant ouverture");
        break;
      }

      bool arrive = deplacerServoVers(ANGLE_OUVERT);
      if (arrive) {
        entrerEtat(ETAT_OUVERT);
        Serial.println("Porte completement ouverte");
      }
      break;
    }

    case ETAT_OUVERT: {
      if (btnUrgAppuye) {
        entrerEtat(ETAT_URGENCE);
        Serial.println("Urgence activee durant attente");
        break;
      }
      if (btnOuvAppuye) {
        tempsDebutAttente = millis();
      }
      if ((millis() - tempsDebutAttente) >= DELAI_ATTENTE_MS) {
        entrerEtat(ETAT_FERMETURE);
        Serial.println("Debut fermeture");
      }
      break;
    }

    case ETAT_FERMETURE: {
      if (btnUrgAppuye) {
        entrerEtat(ETAT_URGENCE);
        Serial.println("Urgence activee durant fermeture");
        break;
      }
      if (btnOuvAppuye) {
        entrerEtat(ETAT_OUVERTURE);
        Serial.println("Bouton ouverture durant fermeture -> reouverture");
        break;
      }

      bool arrive = deplacerServoVers(ANGLE_FERME);
      if (arrive) {
        entrerEtat(ETAT_FERME);
        Serial.println("Porte completement fermee");
      }
      break;
    }

    case ETAT_URGENCE: {
      if (btnUrgAppuye) {
        angleActuel = ANGLE_FERME;
        servo.write(ANGLE_FERME);
        dernierEtatAffiche = (EtatPorte)-1;
        entrerEtat(ETAT_FERME);
        Serial.println("Retour FERME");
      }
      break;
    }
  }
}