#include "Porte.h"

Porte::Porte(int brocheTrig, int brocheEcho, int brocheServo, int brocheBtn)
    : _brocheTrig(brocheTrig),
      _brocheEcho(brocheEcho),
      _brocheServo(brocheServo),
      _brocheBtn(brocheBtn),
      _angleActuel(ANGLE_FERME),
      _etat(PORTE_FERMEE),
      _tempsDebutAttente(0),
      _dernierMouvServo(0),
      _btnEtatPrecedent(HIGH),
      _dernierChangBtn(0),
      _nbClients(0) {}

void Porte::begin() {
    pinMode(_brocheTrig, OUTPUT);
    pinMode(_brocheEcho, INPUT);
    if (_brocheBtn >= 0) pinMode(_brocheBtn, INPUT_PULLUP);
    _servo.attach(_brocheServo);
    _servo.write(ANGLE_FERME);
}

void Porte::update(bool urgenceActivee) {
    bool btnAppuye = _boutonAppuye();

    switch (_etat) {
        case PORTE_FERMEE: {
            if (urgenceActivee) { activerUrgence(); break; }
            int  dist          = _lireDistance();
            bool clientDetecte = (dist > 0 && dist <= DISTANCE_DETECTION_CM);
            if (clientDetecte || btnAppuye) {
                _nbClients++;
                _entrerEtat(PORTE_OUVERTURE);
            }
            break;
        }
        case PORTE_OUVERTURE: {
            if (urgenceActivee) { activerUrgence(); break; }
            if (_deplacerServoVers(ANGLE_OUVERT)) _entrerEtat(PORTE_OUVERTE);
            break;
        }
        case PORTE_OUVERTE: {
            if (urgenceActivee) { activerUrgence(); break; }
            if (btnAppuye) _tempsDebutAttente = millis();
            if ((millis() - _tempsDebutAttente) >= (unsigned long)DELAI_ATTENTE_MS) {
                _entrerEtat(PORTE_FERMETURE);
            }
            break;
        }
        case PORTE_FERMETURE: {
            if (urgenceActivee) { activerUrgence(); break; }
            if (btnAppuye) { _entrerEtat(PORTE_OUVERTURE); break; }
            if (_deplacerServoVers(ANGLE_FERME)) _entrerEtat(PORTE_FERMEE);
            break;
        }
        case PORTE_URGENCE:
            break;
    }
}

void Porte::ouvrirManuellement() {
    if (_etat == PORTE_FERMEE || _etat == PORTE_FERMETURE) {
        _nbClients++;
        _entrerEtat(PORTE_OUVERTURE);
    }
}

void Porte::activerUrgence()    { _entrerEtat(PORTE_URGENCE); }

void Porte::desactiverUrgence() {
    _servo.write(ANGLE_FERME);
    _angleActuel = ANGLE_FERME;
    _entrerEtat(PORTE_FERMEE);
}

EtatPorte Porte::getEtat()      const { return _etat; }
int       Porte::getNbClients() const { return _nbClients; }

int Porte::_lireDistance() {
    digitalWrite(_brocheTrig, LOW);
    delayMicroseconds(2);
    digitalWrite(_brocheTrig, HIGH);
    delayMicroseconds(10);
    digitalWrite(_brocheTrig, LOW);
    long duree = pulseIn(_brocheEcho, HIGH, 30000);
    if (duree == 0) return -1;
    return (int)(duree * 0.034 / 2);
}

bool Porte::_boutonAppuye() {
    if (_brocheBtn < 0) return false;

    bool etat = digitalRead(_brocheBtn);
    if (etat != _btnEtatPrecedent) {
        _dernierChangBtn  = millis();
        _btnEtatPrecedent = etat;
    }
    if ((millis() - _dernierChangBtn) > (unsigned long)DELAI_ANTI_REBOND_MS
        && etat == LOW) {
        _btnEtatPrecedent = HIGH;
        return true;
    }
    return false;
}

bool Porte::_deplacerServoVers(int angleCible) {
    if ((millis() - _dernierMouvServo) < (unsigned long)DELAI_MOUVEMENT_MS) return false;
    _dernierMouvServo = millis();
    if      (_angleActuel < angleCible) _angleActuel++;
    else if (_angleActuel > angleCible) _angleActuel--;
    _servo.write(_angleActuel);
    return (_angleActuel == angleCible);
}

void Porte::_entrerEtat(EtatPorte nouvelEtat) {
    _etat = nouvelEtat;
    if (nouvelEtat == PORTE_OUVERTE) _tempsDebutAttente = millis();
}
