#include "MatriceDisplay.h"

// Smiley pour le mode normal
const uint8_t MatriceDisplay::BITMAP_SMILEY[8] = {
    B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10100101,
    B10011001,
    B01000010,
    B00111100
};

// X pour le mode urgence
const uint8_t MatriceDisplay::BITMAP_X[8] = {
    B10000001,
    B01000010,
    B00100100,
    B00011000,
    B00011000,
    B00100100,
    B01000010,
    B10000001
};

void MatriceDisplay::begin() {
    u8g2.begin();
    u8g2.setContrast(5);
    u8g2.setFont(u8g2_font_4x6_tr);
}

// Fait defiler le texte SPECIAL avec un timer pour une vitesse constante
void MatriceDisplay::afficherRabais(unsigned long currentTime) {
    static unsigned long previousMillis = 0;
    static int positionBits = 0;
    const int  DELAI_DEFILEMENT = 60;
    const int  LARGEUR_POLICE   = 4;
    const char* MESSAGE         = "  SPECIAL  ";

    if (currentTime - previousMillis >= DELAI_DEFILEMENT) {
        previousMillis = currentTime;
        int longueurBits = strlen(MESSAGE) * LARGEUR_POLICE + 8;
        u8g2.clearBuffer();
        u8g2.drawStr(-(positionBits % longueurBits), 7, MESSAGE);
        u8g2.sendBuffer();
        positionBits++;
        if (positionBits >= longueurBits) positionBits = 0;
    }
}

// Affiche un smiley bitmap en mode normal
void MatriceDisplay::afficherNormal() {
    u8g2.clearBuffer();
    u8g2.drawBitmap(0, 0, 1, 8, BITMAP_SMILEY);
    u8g2.sendBuffer();
}

// Fait clignoter la matrice toutes les 300 ms en mode erreur
void MatriceDisplay::afficherErreur(unsigned long currentTime) {
    static unsigned long previousMillis = 0;
    static bool allume = false;
    const int DELAI_CLIGNOTEMENT = 300;

    if (currentTime - previousMillis >= DELAI_CLIGNOTEMENT) {
        previousMillis = currentTime;
        allume = !allume;
        u8g2.clearBuffer();
        if (allume) u8g2.drawBox(0, 0, 8, 8);
        u8g2.sendBuffer();
    }
}

// Affiche un X bitmap en mode urgence
void MatriceDisplay::afficherUrgence() {
    u8g2.clearBuffer();
    u8g2.drawBitmap(0, 0, 1, 8, BITMAP_X);
    u8g2.sendBuffer();
}

// Eteint completement la matrice
void MatriceDisplay::eteindre() {
    u8g2.clearBuffer();
    u8g2.sendBuffer();
}
