# Modifications possibles - FittsController

## Localisation
`/fitts/controller/fittscontroller.h` et `/fitts/controller/fittscontroller.cpp`

---

## 1. Changer la taille du cercle de démarrage

### Avant (initGame)
```cpp
int size = 180;
```

### Après
```cpp
int size = 120; // Plus petit
```

### Résultat
Le cercle bleu "Démarrer" au début du test sera plus petit (120px au lieu de 180px). Le label "Démarrer" pourrait se retrouver mal positionné — ajuster aussi `labelStartProxy->setPos(...)`.

---

## 2. Changer la couleur des cibles

### Avant (nextCible)
```cpp
scene->addEllipse(posX - (size / 2), posY - (size / 2), size, size,
    QPen(QColor(color_red)), QBrush(QColor(color_red)))
    ->setCursor(Qt::PointingHandCursor);
```

### Après
```cpp
QColor targetColor(255, 165, 0); // Orange
scene->addEllipse(posX - (size / 2), posY - (size / 2), size, size,
    QPen(targetColor), QBrush(targetColor))
    ->setCursor(Qt::PointingHandCursor);
```

### Résultat
Les cibles apparaissent en orange au lieu de rouge. Note : `color_red` est défini dans `fittsview.h` — on peut aussi modifier la constante là-bas pour un changement global.

---

## 3. Accepter les clics même en dehors de la cible

### Avant (cibleClicked)
```cpp
if (sqrt(pow(coords.x() - this->fittsModel->cercleCenter.last().x(), 2) +
         pow(coords.y() - this->fittsModel->cercleCenter.last().y(), 2))
    <= this->fittsModel->cercleSize.last() / 2) {
    // Enregistre le temps et passe à la cible suivante
}
```

### Après
```cpp
// Toujours accepter le clic, même hors cible
this->fittsModel->times.append(timer->elapsed());
timer->restart();
this->fittsModel->clickPoints.append(QPoint(x, y));
this->nextCible();
```

### Résultat
Chaque clic est comptabilisé, qu'il soit dans la cible ou non. Le test mesure alors le temps total pour chaque tentative, pas seulement les clics réussis. Cela change la nature de l'expérience.

---

## 4. Ajouter un compteur d'erreurs (clics ratés)

### Avant (cibleClicked - partie else)
```cpp
} else {
    QPointF coords = this->fittsView->graphicView->mapToScene(x, y);
    if (sqrt(...) <= this->fittsModel->cercleSize.last() / 2) {
        this->fittsModel->times.append(timer->elapsed());
        // ...
    }
}
```

### Après
```cpp
} else {
    QPointF coords = this->fittsView->graphicView->mapToScene(x, y);
    if (sqrt(...) <= this->fittsModel->cercleSize.last() / 2) {
        this->fittsModel->times.append(timer->elapsed());
        // ...
    } else {
        this->fittsModel->missCount++; // Ajouter int missCount = 0; dans FittsModel
        qDebug() << "Clic raté ! Total erreurs :" << this->fittsModel->missCount;
    }
}
```

### Résultat
Les clics en dehors de la cible sont comptabilisés. Il faut aussi ajouter `int missCount = 0;` dans `fittsmodel.h` et afficher la valeur dans les résultats.

---

## 5. Modifier la formule de Fitts (temps théorique)

### Avant (calculateResultHome)
```cpp
double F = log2((D / L) + 1);
double value = (this->fittsModel->a * 1000) + (this->fittsModel->b * 1000 * F);
```

### Après — Formule de Shannon
```cpp
double F = log2((2 * D / L) + 1); // Formulation de Shannon
double value = (this->fittsModel->a * 1000) + (this->fittsModel->b * 1000 * F);
```

### Résultat
La courbe théorique utilise la formulation de Shannon (plus courante dans la littérature moderne) au lieu de la formulation originale. Les temps théoriques seront légèrement différents.

---

## 6. Réactiver l'écart type et l'erreur type

### Avant (calculateResultHome - fin)
```cpp
this->fittsModel->diffMoy = diffMoy;
this->fittsModel->avgTime = ...;
```

### Après
```cpp
this->fittsModel->diffMoy = diffMoy;

// Calcul de l'écart type
double variance = 0;
for (int i = 0; i < diffValues.size(); ++i) {
    variance += pow(diffValues[i] - diffMoy, 2);
}
variance /= diffValues.size();
this->fittsModel->ecartType = sqrt(variance);

// Erreur type
this->fittsModel->erreurType = this->fittsModel->ecartType / sqrt(diffValues.size());

// Intervalle de confiance à 95%
this->fittsModel->itc95 = 1.96 * this->fittsModel->erreurType;

this->fittsModel->avgTime = ...;
```

Et dans `displayResults()` décommenter :
```cpp
this->fittsView->ecartType->setText(
    "Écart type = " + QString::number(this->fittsModel->ecartType) + " ms");
this->fittsView->erreurType->setText(
    "Erreur type = " + QString::number(this->fittsModel->erreurType) + " ms");
this->fittsView->itc95->setText(
    "Intervalle de conf à 95% = " + QString::number(this->fittsModel->itc95) + " ms");
```

### Résultat
L'écran de résultats affiche 4 statistiques au lieu de 2 : différence moyenne, écart type, erreur type, et intervalle de confiance à 95%. Il faut aussi décommenter la création des labels dans `fittsview.cpp`.

---

## 7. Changer la vitesse d'animation du cercle de démarrage

### Avant (initGame)
```cpp
animation->setDuration(1500);
animation->setKeyValueAt(0, 0);
animation->setKeyValueAt(0.5, 64);
animation->setKeyValueAt(1, 0);
```

### Après
```cpp
animation->setDuration(800);  // Plus rapide
animation->setKeyValueAt(0, 0);
animation->setKeyValueAt(0.5, 96); // Halo plus large
animation->setKeyValueAt(1, 0);
```

### Résultat
Le halo bleu pulse plus vite (0.8s au lieu de 1.5s) et est plus lumineux (rayon de flou de 96 au lieu de 64).

---

## 8. Modifier la formule du temps théorique Keystroke

### Avant (endKeystrokeEval)
```cpp
int theoreticalDuration = h + (p + k * 2) * depth + h;
```

### Après — Inclure la charge mentale
```cpp
int m = this->keystrokeModel->m;
int theoreticalDuration = m + h + (p + k * 2) * depth + h;
```

### Résultat
Le temps théorique inclut maintenant le temps de réflexion mentale (M) au début de la tâche. Le paramètre M, qui existait déjà dans le modèle mais n'était pas utilisé, est désormais pris en compte.

---

## 9. Limiter le nombre d'entrées dans l'historique JSON

### Avant (addHisto)
```cpp
QJsonArray array = json.array();
array.prepend(this->fittsModel->writeDataJson());
```

### Après
```cpp
QJsonArray array = json.array();
array.prepend(this->fittsModel->writeDataJson());

// Garder seulement les 50 derniers tests
while (array.size() > 50) {
    array.removeLast();
}
```

### Résultat
Le fichier `data.json` ne dépasse jamais 50 entrées. Les tests les plus anciens sont supprimés automatiquement.

---

## 10. Ajouter un son au clic sur une cible

### Avant (cibleClicked - dans le if réussi)
```cpp
this->fittsModel->times.append(timer->elapsed());
timer->restart();
this->fittsModel->clickPoints.append(QPoint(x, y));
this->nextCible();
```

### Après
```cpp
this->fittsModel->times.append(timer->elapsed());
timer->restart();
this->fittsModel->clickPoints.append(QPoint(x, y));

// Jouer un son de clic
QSound::play(":/sounds/click.wav"); // Ajouter #include <QSound> et le fichier dans les ressources

this->nextCible();
```

### Résultat
Un son est joué à chaque clic réussi sur une cible. Il faut ajouter le fichier audio dans les ressources Qt et inclure le module `multimedia`.
