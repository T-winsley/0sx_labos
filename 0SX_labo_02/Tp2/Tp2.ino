// numéro admission : 2421657
const int potentiometerPin = A1;
const int buttonPin = 2;
const int ledPins[4] = {7, 8, 9, 10};


// config selon numéro étudiant
const bool avantDernierPair = false; // 5 → impair
const bool dernierPair = false;      // 7 → impair


// variables pour gestion bouton
int lastButtonState = LOW;
int buttonState;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;


// setup : exécuté une seule fois au démarrage
void setup() {

  Serial.begin(9600);
  setupLEDs();

  pinMode(buttonPin, INPUT_PULLUP);
}


// loop : s’exécute en continu
void loop() {

  // lire la position du potentiomètre (0 à 20)
  int scaleValue = lirePotentiometre();

  // convertir cette valeur en pourcentage (0 à 100)
  int percent = convertirPourcentage(scaleValue);

  // mettre à jour les dels en temps réel
  gererLEDs(percent);

  // vérifier si bouton appuyé pour afficher la barre
  gererBouton(scaleValue, percent);
}


// initialisation des dels
// configure chaque del comme sortie
void setupLEDs() {
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}


// lecture potentiomètre
// lit valeur analogique (0 à 1023)
// puis convertit en échelle 0 à 20
int lirePotentiometre() {
  int valeur = analogRead(potentiometerPin );
  return map(valeur, 0, 1023, 0, 20);
}


// conversion en pourcentage
// transforme 0–20 en 0–100%
int convertirPourcentage(int scaleValue) {
  return map(scaleValue, 0, 20, 0, 100);
}


void gererBouton(int scaleValue, int percent) {

  // lecture instantanée du bouton
  int reading = digitalRead(buttonPin);

  // si l'état change → on reset le timer anti-rebond
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  // si assez de temps passé → état stable
  if (millis() - lastDebounceTime > debounceDelay) {

    // si nouvel état confirmé
    if (reading != buttonState) {
      buttonState = reading;

      // si bouton pressé → afficher barre
      if (buttonState == LOW) {
        afficherBarre(scaleValue, percent);
      }
    }
  }

  // sauvegarde pour prochaine lecture
  lastButtonState = reading;
}


// affichage barre de progression
void afficherBarre(int valeur, int percent) {

  // affiche pourcentage
  Serial.print(percent);
  Serial.print("% [");

  // dessine la barre (20 caractères)
  for (int i = 0; i < 20; i++) {
    if (i < valeur){
      Serial.print("-");
    }
    else{
     Serial.print(".");
    }
  }

  Serial.println("]");
}


// gestion des dels selon pourcentage
void gererLEDs(int percent) {

  int zone;

  if (percent <= 25){
    zone = 0;
  }
  else if (percent <= 50){
    zone = 1;
  }
  else if (percent <= 75){
    zone = 2;
  }
  else {
    zone = 3;
  }

  // allume progressivement les dels
  for (int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], (i <= zone) ? HIGH : LOW);
  }
}
