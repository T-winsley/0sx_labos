#pragma once

#include <Arduino.h>
#include <Servo.h>

enum EtatPorte {
    PORTE_FERMEE,
    PORTE_OUVERTURE,
    PORTE_OUVERTE,
    PORTE_FERMETURE,
    PORTE_URGENCE
};

class Porte {
public:
    Porte(int brocheTrig, int brocheEcho, int brocheServo, int brocheBtn);

    void begin();
    void update(bool urgenceActivee);
    void ouvrirManuellement();
    void activerUrgence();
    void desactiverUrgence();

    EtatPorte getEtat()      const;
    int       getNbClients() const;

private:
    int _brocheTrig, _brocheEcho, _brocheServo, _brocheBtn;

    Servo         _servo;
    int           _angleActuel;
    EtatPorte     _etat;
    unsigned long _tempsDebutAttente;
    unsigned long _dernierMouvServo;
    bool          _btnEtatPrecedent;
    unsigned long _dernierChangBtn;
    int           _nbClients;

    static const int  DISTANCE_DETECTION_CM = 5;
    static const int  ANGLE_FERME           = 10;
    static const int  ANGLE_OUVERT          = 170;
    static const long DELAI_ATTENTE_MS      = 10000;
    static const long DELAI_MOUVEMENT_MS    = 15;
    static const long DELAI_ANTI_REBOND_MS  = 50;

    int  _lireDistance();
    bool _boutonAppuye();
    bool _deplacerServoVers(int angleCible);
    void _entrerEtat(EtatPorte nouvelEtat);
};
