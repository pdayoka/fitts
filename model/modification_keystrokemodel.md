# Modifications possibles - KeystrokeModel

## Localisation
`/fitts/model/keystrokemodel.h` et `/fitts/model/keystrokemodel.cpp`

---

## 1. Changer les durées par défaut des opérateurs

### Avant
```cpp
int k = 200;
int p = 1100;
int h = 400;
int m = 1300;
```

### Après — Valeurs pour un utilisateur expert
```cpp
int k = 120;   // Dactylographe rapide
int p = 800;   // Pointage rapide
int h = 300;   // Transition rapide
int m = 900;   // Temps de réflexion réduit
```

### Résultat
Les temps théoriques calculés seront plus courts, correspondant à un utilisateur expérimenté. Par exemple, pour une profondeur de 3 :
- Avant : `400 + (1100 + 200×2) × 3 + 400 = 5300 ms`
- Après : `300 + (800 + 120×2) × 3 + 300 = 3720 ms`

---

## 2. Initialiser lastMeasuredTime

### Avant
```cpp
int lastMeasuredTime;
```

### Après
```cpp
int lastMeasuredTime = 0;
```

### Résultat
Évite un comportement indéfini si on accède à `lastMeasuredTime` avant le premier test. La valeur est initialisée à 0 au lieu d'une valeur aléatoire.

---

## 3. Ajouter un historique des tests

### Avant
```cpp
class KeystrokeModel {
public:
    KeystrokeModel();
    int lastMeasuredTime;
    int k = 200;
    int p = 1100;
    int h = 400;
    int m = 1300;
};
```

### Après
```cpp
#include <QList>

struct KeystrokeResult {
    int measuredTime;      // Temps réel
    int theoreticalTime;   // Temps calculé
    int depth;             // Profondeur du fichier
    QString filePath;      // Chemin du fichier cliqué
};

class KeystrokeModel {
public:
    KeystrokeModel();
    int lastMeasuredTime = 0;
    int k = 200;
    int p = 1100;
    int h = 400;
    int m = 1300;

    QList<KeystrokeResult> history; // Historique des tests
};
```

### Résultat
Chaque test Keystroke est conservé en mémoire. On peut ensuite afficher un tableau récapitulatif des tests précédents et faire des statistiques (moyenne, progression...).

---

## 4. Ajouter la sérialisation JSON

### Avant (keystrokemodel.cpp)
```cpp
KeystrokeModel::KeystrokeModel() {}
```

### Après
```cpp
#include <QJsonObject>

KeystrokeModel::KeystrokeModel() {}

QJsonObject KeystrokeModel::writeDataJson() {
    QJsonObject obj;
    obj["k"] = k;
    obj["p"] = p;
    obj["h"] = h;
    obj["m"] = m;
    obj["lastMeasuredTime"] = lastMeasuredTime;
    return obj;
}
```

Et dans le `.h` :
```cpp
#include <QJsonObject>
// ...
public:
    QJsonObject writeDataJson();
```

### Résultat
Les résultats Keystroke peuvent être sauvegardés dans un fichier JSON, comme c'est déjà fait pour les tests Fitts. On peut ensuite appeler cette méthode dans le contrôleur pour persister les données.

---

## 5. Ajouter un opérateur R (temps de réponse système)

### Avant
```cpp
int k = 200;
int p = 1100;
int h = 400;
int m = 1300;
```

### Après
```cpp
int k = 200;
int p = 1100;
int h = 400;
int m = 1300;
int r = 500; // Temps de réponse du système (ouverture de dossier)
```

Et dans `fittscontroller.cpp` (`endKeystrokeEval`) :
```cpp
// Avant
int theoreticalDuration = h + (p + k * 2) * depth + h;

// Après — chaque ouverture de dossier a un temps de réponse
int r = this->keystrokeModel->r;
int theoreticalDuration = h + (p + k * 2 + r) * depth + h;
```

### Résultat
Le modèle inclut le temps de réponse du système pour chaque niveau de profondeur. Le temps théorique augmente de `r × profondeur` ms. Il faut aussi ajouter un `QSpinBox` dans la vue pour le paramètre R.

---

## 6. Ajouter un mode "clavier uniquement"

### Avant — Formule souris uniquement
Dans `fittscontroller.cpp` :
```cpp
int theoreticalDuration = h + (p + k * 2) * depth + h;
```

### Après — Ajouter un booléen pour le mode clavier
Dans `keystrokemodel.h` :
```cpp
bool keyboardMode = false; // false = souris, true = clavier
```

Dans `fittscontroller.cpp` :
```cpp
int theoreticalDuration;
if (this->keystrokeModel->keyboardMode) {
    // Clavier : navigation avec flèches + Entrée
    // Encodage : M H (K K)^profondeur K
    int m = this->keystrokeModel->m;
    theoreticalDuration = m + h + (k * 2) * depth + k;
} else {
    // Souris : H (PK)^(2*profondeur) H
    theoreticalDuration = h + (p + k * 2) * depth + h;
}
```

### Résultat
L'utilisateur peut comparer le temps théorique entre l'utilisation de la souris et du clavier pour naviguer dans l'arborescence. Le mode clavier est généralement plus rapide car il n'y a pas de pointage (P).
