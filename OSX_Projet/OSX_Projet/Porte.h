#pragma once

#include <Arduino.h>
#include <Servo.h>

/*
 * EtatPorte
 * ---------
 * Machine a etats de la porte automatique.
 */
enum EtatPorte {
    PORTE_FERMEE,
    PORTE_OUVERTURE,
    PORTE_OUVERTE,
    PORTE_FERMETURE,
    PORTE_URGENCE
};

/*
 * Porte
 * -----
 * Gere la porte automatique avec servo moteur et capteur de distance HC-SR04.
 * La porte s'ouvre si un client est detecte a moins de DISTANCE_DETECTION_CM.
 * Le bouton permet aussi d'ouvrir manuellement.
 */
class Porte {
public:
    Porte(int brocheTrig, int brocheEcho, int brocheServo, int brocheBtn);

    void begin();
    // urgenceActivee : true si le bouton urgence global vient d'etre presse
    void update(bool urgenceActivee);

    void ouvrirManuellement(); // Ouverture via bouton
    void activerUrgence();
    void desactiverUrgence();

    EtatPorte getEtat()      const;
    int       getNbClients() const;

private:
    int _brocheTrig;
    int _brocheEcho;
    int _brocheServo;
    int _brocheBtn;

    Servo         _servo;
    int           _angleActuel;
    EtatPorte     _etat;
    unsigned long _tempsDebutAttente;
    unsigned long _dernierMouvServo;
    bool          _btnEtatPrecedent;
    unsigned long _dernierChangBtn;
    int           _nbClients;

    static const int  DISTANCE_DETECTION_CM = 5;  // 20cm comme le labo
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
