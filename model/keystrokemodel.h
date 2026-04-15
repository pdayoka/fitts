#ifndef KEYSTROKEMODEL_H
#define KEYSTROKEMODEL_H

#include <QString>

class KeystrokeModel {
public:
	KeystrokeModel();
	
    // Dernier temps expérimental relevé
	int lastMeasuredTime;
	
    // Durées du modèle Keystroke, valeurs par défaut paramètrables depuis l'interface
	int k = 200;
	int p = 1100;
	int h = 400;
	int m = 1300;
};

#endif // KEYSTROKEMODEL_H
