# Modifications possibles - FittsModel

## Localisation
`/fitts/model/fittsmodel.h` et `/fitts/model/fittsmodel.cpp`

---

## 1. Changer les valeurs par défaut des paramètres

### Avant
```cpp
double a = 0.20;
double b = 0.10;
int nbCible = 10;
int minSize = 10;
int maxSize = 160;
```

### Après
```cpp
double a = 0.15;
double b = 0.08;
int nbCible = 15;
int minSize = 20;
int maxSize = 200;
```

### Résultat
Le test démarre avec des paramètres différents : 15 cibles au lieu de 10, cibles plus grandes (20-200px), et des constantes a/b ajustées. La courbe théorique sera plus basse (temps prédits plus courts).

---

## 2. Corriger la fuite mémoire dans writeDataJson

### Avant
```cpp
QDateTime *currentDate = new QDateTime;
dataItem["dateTime"] = currentDate->currentDateTime().toString("dd/MM/yyyy - hh:mm");
```

### Après
```cpp
dataItem["dateTime"] = QDateTime::currentDateTime().toString("dd/MM/yyyy - hh:mm");
```

### Résultat
Plus de fuite mémoire. Le comportement est identique : la date/heure du test est enregistrée dans le JSON. On utilise directement la méthode statique sans allocation inutile.

---

## 3. Ajouter la sérialisation des temps dans le JSON

### Avant (writeDataJson)
```cpp
dataItem["cercleSize"] = cercleSizeArray;

QDateTime *currentDate = new QDateTime;
// ...
```

### Après
```cpp
dataItem["cercleSize"] = cercleSizeArray;

QJsonArray timesArray;
for (int i = 0; i < times.size(); i++) {
    timesArray.append(static_cast<qint64>(times[i]));
}
dataItem["times"] = timesArray;

dataItem["dateTime"] = QDateTime::currentDateTime().toString("dd/MM/yyyy - hh:mm");
```

### Résultat
Le fichier JSON contient maintenant les temps de chaque clic :
```json
{
  "times": [432, 567, 312, 890, ...],
  "clickPoints": [...],
  ...
}
```
Utile pour analyser les données dans un outil externe (Excel, Python...).

---

## 4. Ajouter un compteur d'erreurs

### Avant (fittsmodel.h)
```cpp
QList<qint64> times;

friend FittsView;
```

### Après
```cpp
QList<qint64> times;
int missCount = 0; // Nombre de clics ratés

friend FittsView;
```

Et dans `writeDataJson()`, ajouter :
```cpp
dataItem["missCount"] = missCount;
```

### Résultat
Le modèle stocke le nombre de clics en dehors des cibles. Cette donnée est sauvegardée dans le JSON. Il faut aussi modifier `FittsController::cibleClicked()` pour incrémenter ce compteur (voir `modification_controller.md`).

---

## 5. Remplacer les friend classes par des getters/setters

### Avant
```cpp
class FittsModel {
public:
    FittsModel();
    // ...
private:
    int nbCible = 10;
    double a = 0.20;
    // ...
    friend FittsView;
    friend FittsController;
};
```

### Après
```cpp
class FittsModel {
public:
    FittsModel();

    int getNbCible() const { return nbCible; }
    void setNbCible(int val) { nbCible = val; }

    double getA() const { return a; }
    void setA(double val) { a = val; }

    double getB() const { return b; }
    void setB(double val) { b = val; }

    int getMinSize() const { return minSize; }
    void setMinSize(int val) { minSize = val; }

    int getMaxSize() const { return maxSize; }
    void setMaxSize(int val) { maxSize = val; }

    int getCibleLeft() const { return cibleLeft; }
    void setCibleLeft(int val) { cibleLeft = val; }

    double getDiffMoy() const { return diffMoy; }
    void setDiffMoy(double val) { diffMoy = val; }

    double getAvgTime() const { return avgTime; }
    void setAvgTime(double val) { avgTime = val; }

    QList<QPoint>& getClickPoints() { return clickPoints; }
    QList<QPoint>& getCercleCenter() { return cercleCenter; }
    QList<int>& getCercleSize() { return cercleSize; }
    QList<qint64>& getTimes() { return times; }

    // ...
private:
    int nbCible = 10;
    double a = 0.20;
    // ...
    // Supprimer les friend
};
```

### Résultat
Meilleur encapsulage : on contrôle l'accès aux données. **Attention** : il faut ensuite modifier **tout** le contrôleur et la vue pour utiliser les getters/setters au lieu de l'accès direct (`this->fittsModel->a` → `this->fittsModel->getA()`). C'est un gros refactoring.

---

## 6. Changer le format de date dans le JSON

### Avant
```cpp
dataItem["dateTime"] = currentDate->currentDateTime().toString("dd/MM/yyyy - hh:mm");
```

### Après — Format ISO 8601
```cpp
dataItem["dateTime"] = QDateTime::currentDateTime().toString(Qt::ISODate);
```

### Résultat
La date passe de `"01/04/2026 - 14:30"` à `"2026-04-01T14:30:00"`. Format standard, plus facile à parser par d'autres outils.

---

## 7. Ajouter une validation des paramètres

### Avant
```cpp
FittsModel::FittsModel() {}
```

### Après
```cpp
FittsModel::FittsModel() {}

bool FittsModel::isValid() const {
    if (minSize >= maxSize) return false;
    if (nbCible < 1) return false;
    if (a < 0 || b < 0) return false;
    return true;
}
```

Et dans le `.h` :
```cpp
public:
    bool isValid() const;
```

### Résultat
On peut vérifier la cohérence des paramètres avant de lancer un test. Appeler `if (!fittsModel->isValid()) { /* afficher erreur */ }` dans le contrôleur avant `initEvalScreen()`.
