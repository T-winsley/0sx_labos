#pragma once

#include <U8g2lib.h>

/*
 * MatriceDisplay
 * --------------
 * Gere l'affichage sur la matrice LED MAX7219 8x8 via la librairie U8g2.
 */
class MatriceDisplay {
private:
    U8G2_MAX7219_8X8_F_4W_SW_SPI u8g2;

    // Bitmaps des icones
    static const uint8_t BITMAP_SMILEY[8];
    static const uint8_t BITMAP_X[8];

public:
    MatriceDisplay(int clk, int din, int cs)
        : u8g2(U8G2_R0, clk, din, cs, U8X8_PIN_NONE, U8X8_PIN_NONE) {}

    void begin();

    // afficherRabais prend le temps courant pour le defilement non-bloquant
    void afficherRabais(unsigned long currentTime);
    void afficherNormal();
    void afficherErreur(unsigned long currentTime);
    void afficherUrgence();
    void eteindre();
};
