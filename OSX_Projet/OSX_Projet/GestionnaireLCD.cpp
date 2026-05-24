#include "GestionnaireLCD.h"

GestionnaireLCD::GestionnaireLCD(Porte& porte, Conveyor& convoyeur, SystemeControle& systeme)
    : _lcd(0x27, 16, 2),
      _porte(porte),
      _convoyeur(convoyeur),
      _systeme(systeme),
      _pageCourante(0),
      _dernierChangPage(0),
      _urgenceAffichee(false) {}

void GestionnaireLCD::begin() {
    _lcd.begin();
    _lcd.backlight();
    _lcd.clear();
}

void GestionnaireLCD::update(bool enUrgence) {
    if (enUrgence) {
        if (!_urgenceAffichee) {
            _lcd.clear();
            _afficherPageUrgence();
            _urgenceAffichee = true;
        }
        return;
    }

    if (_urgenceAffichee) {
        _urgenceAffichee  = false;
        _dernierChangPage = 0;
    }

    if ((millis() - _dernierChangPage) < (unsigned long)DELAI_PAGE_MS) return;
    _dernierChangPage = millis();

    _lcd.clear();
    switch (_pageCourante) {
        case 0: _afficherPagePorte();     break;
        case 1: _afficherPageConvoyeur(); break;
        case 2: _afficherPageMatrice();   break;
    }
    _pageCourante = (_pageCourante + 1) % 3;
}

void GestionnaireLCD::_afficherPagePorte() {
    _lcd.setCursor(0, 0);
    _lcd.print("Porte:          ");
    _lcd.setCursor(0, 1);
    switch (_porte.getEtat()) {
        case PORTE_FERMEE:    _lcd.print("Fermee          "); break;
        case PORTE_OUVERTURE: _lcd.print("En ouverture... "); break;
        case PORTE_OUVERTE:   _lcd.print("Ouverte         "); break;
        case PORTE_FERMETURE: _lcd.print("En fermeture... "); break;
        default:              _lcd.print("---             "); break;
    }
}

void GestionnaireLCD::_afficherPageConvoyeur() {
    _lcd.setCursor(0, 0);
    bool actif = (_convoyeur.getState() == CONV_AVANCE || _convoyeur.getState() == CONV_RECULE);
    _lcd.print(actif ? "Conv:OUI " : "Conv:NON ");
    if      (_convoyeur.getState() == CONV_AVANCE) _lcd.print("AVANT  ");
    else if (_convoyeur.getState() == CONV_RECULE) _lcd.print("ARR    ");
    else                                           _lcd.print("STOP   ");

    _lcd.setCursor(0, 1);
    _lcd.print("Vitesse: ");
    _lcd.print(_convoyeur.getSpeed());
    _lcd.print("      ");
}

void GestionnaireLCD::_afficherPageMatrice() {
    _lcd.setCursor(0, 0);
    _lcd.print("Mode affiche:   ");
    _lcd.setCursor(0, 1);
    _lcd.print(_systeme.getNomMode());
    _lcd.print("          ");
}

void GestionnaireLCD::_afficherPageUrgence() {
    _lcd.setCursor(0, 0);
    _lcd.print("!! URGENCE !!   ");
    _lcd.setCursor(0, 1);
    _lcd.print("Systeme arrete  ");
}
