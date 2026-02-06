// constantes
const int LED_PIN = 13;
const char ETUDIANT[] = "2142752";

void setup() {

  pinMode(LED_PIN, OUTPUT);

  // Sert à afficher l’état du programme
  Serial.begin(9600);
}

void loop() {

  // ÉTAT 1: ON / OFF
  // Affiche l’état actuel et le numéro d’étudiant
  Serial.print("Etat : Allume – ");
  Serial.println(ETUDIANT);

  // Éteint la LED pendant 300 ms
  digitalWrite(LED_PIN, LOW);
  delay(300);

  // Allume la LED pendant 2000 ms
  digitalWrite(LED_PIN, HIGH);
  delay(2000);

  // Éteint la LED pendant 1000 ms
  digitalWrite(LED_PIN, LOW);
  delay(1000);

  
  // ÉTAT 2: VARIATION
  // Affiche l’état actuel et le numéro d’étudiant 
  Serial.print("Etat : Varie – ");
  Serial.println(ETUDIANT);

  // Nombre de niveaux d’intensité possibles (0 à 255)
  int steps = 255;

  // Temps d’attente entre chaque variation
  int delayTime = 2048 / steps;

  // Augmente progressivement l’intensité de la LED
  for (int i = 0; i <= 255; i++) {

    // Modifie l’intensité lumineuse de la LED
    analogWrite(LED_PIN, i);

    // Pause entre chaque niveau d’intensité
    delay(delayTime);
  }

  // ÉTAT 3: CLIGNOTEMENT
  // Affiche l’état actuel et le numéro d’étudiant
  Serial.print("Etat : Clignotement – ");
  Serial.println(ETUDIANT);

  // Nombre de clignotements à effectuer
  int clignotements = 3;

  for (int i = 0; i < clignotements; i++) {

    // Allume la LED pendant 350 ms
    digitalWrite(LED_PIN, HIGH);
    delay(350);

    // Éteint la LED pendant 350 ms
    digitalWrite(LED_PIN, LOW);
    delay(350);
  }

}
