#pragma once

#include <U8g2lib.h>

// Affichage matrice LED MAX7219 8x8 via U8g2 (SW SPI)
class MatriceDisplay {
public:
    MatriceDisplay(int clk, int din, int cs)
        : u8g2(U8G2_R0, clk, din, cs, U8X8_PIN_NONE, U8X8_PIN_NONE) {}

    void begin();
    void afficherRabais(unsigned long currentTime);
    void afficherNormal();
    void afficherErreur(unsigned long currentTime);
    void afficherUrgence();
    void eteindre();

private:
    U8G2_MAX7219_8X8_F_4W_SW_SPI u8g2;

    static const uint8_t BITMAP_SMILEY[8];
    static const uint8_t BITMAP_X[8];
};
