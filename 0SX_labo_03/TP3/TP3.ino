#include <Wire.h>
#include <LCD_I2C.h>

LCD_I2C lcd(0x27, 16, 2);

// Broches
const int PIN_LDR     = A0;
const int PIN_JOY_X   = A1;
const int PIN_JOY_Y   = A2;
const int PIN_JOY_BTN = 2;
const int PIN_LED     = 8;

// Caractère personnalisé « 57 »
byte customChar[8] = {
  0b11111,
  0b10000,
  0b11111,
  0b00001,
  0b11111,
  0b00000,
  0b00111,
  0b00100
};

// LDR
float         luminosite    = 0.0;
bool          lightOn       = false;
unsigned long lightStart    = 0;
bool          timerActif    = false;
const int     LDR_SEUIL     = 30;
const long    DELAI_LUMIERE = 5000;

// Drone
float         altitude      = 0.0;
float         angle         = 0.0;
const float   ALT_MAX       = 200.0;
const float   VITESSE       = 1.0;

// Timing général — un seul unsigned long réutilisé partout
unsigned long dernierUpdate = 0;
unsigned long dernierSerial = 0;

// Bouton
volatile bool btnFlag    = false;
unsigned long dernierBtn = 0;
const long    DEBOUNCE   = 200;
int           page       = 0;

// Intervalles — regroupés
const long INTERVALLE_ALTITUDE = 100;
const long INTERVALLE_SERIAL   = 100;

// Déclenchée automatiquement par interruption matérielle quand le bouton est pressé. Évite de manquer un clic dans la loop.
void ISR_bouton() {
  unsigned long now = millis();
  if (now - dernierBtn > DEBOUNCE) { 
    btnFlag = true; 
    dernierBtn = now; 
  }
}

// Affiche le nom, le caractère personnalisé et le numéro masqué pendant 3s au démarrage.
void afficherDemarrage() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("TENE");
  lcd.setCursor(0, 1); lcd.write(byte(0));
  lcd.setCursor(6, 1); lcd.print("*****57");
  delay(3000);
  lcd.clear();
}

// Bascule entre la page 0 et la page 1 à chaque clic du bouton.
void gererBouton() {
  if (btnFlag) { 
    btnFlag = false; 
    page = 1 - page;
    lcd.clear(); 
  }
}

// Lit la LDR sur A0 et convertit la valeur en pourcentage.
void lireLDR() {
  luminosite = (analogRead(PIN_LDR) / 1023.0) * 100.0;
}

// Allume ou éteint la LED après 5s consécutives sous/sur le seuil.
void gererProjecteurs() {
  unsigned long now = millis();
  bool condition = lightOn ? (luminosite > LDR_SEUIL) : (luminosite <= LDR_SEUIL);
  if (condition) {
    if (!timerActif) { 
      timerActif = true; lightStart = now;
    }else if (now - lightStart >= DELAI_LUMIERE) {
      lightOn    = !lightOn;
      timerActif = false;
      digitalWrite(PIN_LED, lightOn ? HIGH : LOW);
    }
  } else { timerActif = false; }
}

// Lit le joystick et met à jour l'altitude (axe Y) et l'angle de direction (axe X) du drone.
void lireJoystick(int &rawX, int &rawY) {
  unsigned long now = millis();
  rawX  = analogRead(PIN_JOY_X);
  rawY  = analogRead(PIN_JOY_Y);
  angle = ((rawX - 512) / 512.0) * 90.0;

  if (now - dernierUpdate >= INTERVALLE_ALTITUDE) {
    float delta = (float)(now - dernierUpdate) / 1000.0 * VITESSE;
    if (rawY < 412) {
      altitude = min(altitude + delta, ALT_MAX);
    }else if (rawY > 612){
    altitude = max(altitude - delta, 0.0f);
    }
    dernierUpdate = now;
  }
}

// Affiche l'état vertical (UP/DOWN/STAB), l'altitude et la direction en degrés avec (G) ou (D).
void afficherPageVitesse(int rawY) {
  char buf[7];
  lcd.setCursor(0, 0);
  if(rawY < 412){
    lcd.print("UP  ");
  } else if (rawY > 612) {
    lcd.print("DOWN ");
  }
  else {
    lcd.print("STAB ");
  }
  dtostrf(altitude, 4, 0, buf);
  lcd.print("Alt:"); lcd.print(buf); lcd.print("m   ");

  lcd.setCursor(0, 1);
  dtostrf(abs(angle), 4, 1, buf);
  lcd.print("Dir:"); lcd.print(buf);
  lcd.print(angle >= 0 ? "(D)  " : "(G)  ");
}

// Affiche la luminosité en % et l'état des projecteurs.
void afficherPageCapteur() {
  char buf[7];
  dtostrf(luminosite, 5, 1, buf);
  lcd.setCursor(0, 0); lcd.print("Lum:"); lcd.print(buf); lcd.print("%  ");
  lcd.setCursor(0, 1); lcd.print("LIGHT: ");
  lcd.print(lightOn ? "ON  " : "OFF  ");
}


// Envoie une trame série toutes les 100ms avec le numéro étudiant, les valeurs brutes du joystick et l'état de la LED.
void envoyerSerial(int rawX, int rawY) {
  unsigned long now = millis();
  if (now - dernierSerial >= INTERVALLE_SERIAL) {
    dernierSerial = now;
    Serial.print("etd:2421657");
    Serial.print(",x:");   Serial.print(rawX);
    Serial.print(",y:");   Serial.print(rawY);
    Serial.print(",sys:"); Serial.println(lightOn ? 1 : 0);
  }
}

// Initialise les broches, le LCD, le port série, l'interruption et affiche l'écran de démarrage.
void setup() {
  Serial.begin(115200);
  pinMode(PIN_JOY_BTN, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  attachInterrupt(digitalPinToInterrupt(PIN_JOY_BTN), ISR_bouton, FALLING);
  lcd.begin();
  lcd.backlight();
  lcd.createChar(0, customChar);
  afficherDemarrage();
  dernierUpdate = millis();
}

// Appelle chaque fonction dans l'ordre logique du programme.
void loop() {
  int rawX, rawY;
  gererBouton();
  lireLDR();
  gererProjecteurs();
  lireJoystick(rawX, rawY);
  if (page == 0) {
    afficherPageVitesse(rawY);
  }
  else{
    afficherPageCapteur();
  }
  envoyerSerial(rawX, rawY);
}