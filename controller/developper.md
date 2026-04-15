# Documentation Développeur - FittsController

## Localisation
`/fitts/controller/fittscontroller.h` et `/fitts/controller/fittscontroller.cpp`

## Rôle du fichier
C'est le **contrôleur principal** de l'application (pattern MVC). Il orchestre toute la logique métier : gestion du test de Fitts, calcul des résultats, gestion de l'historique JSON, et le test Keystroke.

## Attributs

| Attribut | Type | Rôle |
|---|---|---|
| `fittsModel` | `FittsModel*` | Modèle contenant les données du test Fitts |
| `fittsView` | `FittsView*` | Vue principale de l'application |
| `keystrokeModel` | `KeystrokeModel*` | Modèle pour le test Keystroke |
| `histModel` | `QList<FittsModel>*` | Historique des tests en mémoire |
| `timer` | `QElapsedTimer*` | Chronomètre pour mesurer les temps de clic |

## Fonctions

### Constructeur / Destructeur

#### `FittsController()`
Initialise l'application MVC :
1. Crée le `FittsModel`
2. Crée la `FittsView` en lui passant le contrôleur et le modèle
3. Crée la liste d'historique et le `KeystrokeModel`
4. Affiche la fenêtre (`show()`)

#### `~FittsController()`
Libère la mémoire du modèle, de la vue et de l'historique. Note : le `delete keystrokeModel` est commenté (fuite mémoire potentielle).

### Navigation entre écrans

#### `initEvalScreen()`
Passe à l'écran de test :
- Réinitialise le compteur de cibles restantes
- Vide les données précédentes (positions, tailles, temps)
- Met à jour le message de test
- Lance `initGame()` pour afficher le cercle de démarrage

#### `initResultsScreen()`
Passe à l'écran de résultats :
- Calcule les graphiques (`calculateResultHome()`)
- Sauvegarde dans l'historique JSON (`addHisto()`)
- Affiche les statistiques (`displayResults()`)

#### `initSettingsScreen()`
Affiche simplement l'écran de paramètres.

### Logique du test de Fitts

#### `initGame()`
Prépare la scène de test :
- Vérifie que `maxSize` ne dépasse pas la moitié de la vue
- Place un **cercle bleu** de 180px au centre avec un effet de halo animé (pulsation)
- Ajoute un label "Démarrer" à côté du cercle
- L'animation utilise `QPropertyAnimation` sur le `blurRadius` avec une boucle infinie

```cpp
QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
effect->setBlurRadius(32);
// Animation pulsante : 0 → 64 → 0 en 1.5s, en boucle
```

#### `cibleClicked(int x, int y)`
Appelé à chaque clic dans la zone de test :
- **Premier clic** : démarre le chronomètre et passe à la première cible
- **Clics suivants** : vérifie si le clic est dans le cercle rouge (calcul de distance euclidienne). Si oui :
  - Stocke le temps écoulé
  - Redémarre le chrono
  - Stocke la position du clic
  - Passe à la cible suivante

```cpp
// Vérification : distance entre clic et centre <= rayon
sqrt(pow(coords.x() - center.x(), 2) + pow(coords.y() - center.y(), 2)) <= size / 2
```

#### `nextCible()`
Génère et affiche la prochaine cible rouge :
- Décrémente le compteur de cibles
- Si terminé → appelle `finish()`
- Sinon : génère une taille aléatoire entre `minSize` et `maxSize`
- Place le cercle rouge à une position aléatoire dans la scène

#### `finish()`
Désactive la vue graphique et bascule vers l'écran de résultats.

### Calcul des résultats

#### `calculateResultHome()`
Construit les deux graphiques de résultats :

**Graphique 1 — Temps par cible :**
- Courbe rouge : temps expérimental mesuré par cible
- Courbe bleue : temps théorique calculé par la loi de Fitts

**Graphique 2 — Temps vs distance relative :**
- Points rouges : données expérimentales (scatter)
- Courbe bleue : modèle théorique

**Formule de Fitts utilisée :**
```
T = a × 1000 + b × 1000 × log2(D/L + 1)
```
Où `D` = distance euclidienne entre le clic précédent et le centre de la cible, `L` = diamètre de la cible.

**Statistiques calculées :**
- `diffMoy` : différence moyenne entre temps théorique et expérimental
- `avgTime` : temps moyen expérimental

#### `displayResults()`
Met à jour les labels de statistiques dans la vue (`diffMoy`, `avgTime`). Les labels `ecartType`, `erreurType` et `itc95` sont commentés.

### Historique JSON

#### `addHisto()`
Sauvegarde le test courant :
1. Ajoute le modèle en tête de la liste mémoire
2. Lit le fichier `data.json` existant (dans `QStandardPaths::ConfigLocation`)
3. Ajoute le nouveau test en tête du tableau JSON
4. Réécrit le fichier

#### `getHisto()`
Lit et retourne le tableau JSON depuis `data.json`.

#### `openJsonFile()`
Ouvre le fichier `data.json` dans l'application par défaut du système.

### Aide

#### `showHelp()`
Affiche une `QMessageBox` avec :
- Explication de la loi de Fitts
- Image de la formule (`:/img/formule`)
- Déroulement du test
- Explication des résultats

### Test Keystroke

#### `loadKeystrokeSettings()`
Affiche l'écran de paramètres Keystroke et charge les valeurs depuis le modèle dans les spinboxes.

#### `saveKeystrokeSettings()`
Lit les valeurs des spinboxes et les enregistre dans le `KeystrokeModel`, puis bascule vers l'écran Keystroke.

#### `startKeystrokeEval()`
Démarre le test Keystroke :
- Affiche un `QFileSystemModel` basé sur le répertoire courant
- Démarre le chronomètre

#### `endKeystrokeEval(const QModelIndex &index)`
Termine le test Keystroke au double-clic sur un élément :
- Calcule la profondeur de l'élément dans l'arborescence
- Calcule le temps théorique avec la formule : `T = H + (P + K × 2) × profondeur + H`
- Affiche la comparaison temps réel vs temps théorique

### Slots de modification des paramètres

| Slot | Action |
|---|---|
| `aValueChanged(double)` | Met à jour `a`, recalcule et affiche les résultats |
| `bValueChanged(double)` | Met à jour `b`, recalcule et affiche les résultats |
| `nbCibleChanged(int)` | Met à jour le nombre de cibles, recalcule et affiche |
| `minSizeChanged(int)` | Met à jour la taille minimum |
| `maxSizeChanged(int)` | Met à jour la taille maximum |

## Ce qu'on peut modifier

### Ajouter une statistique (ex: écart type)
Les lignes sont déjà prévues mais commentées dans `displayResults()` :
```cpp
// Décommenter et calculer dans calculateResultHome() :
this->fittsView->ecartType->setText("Écart type = " + QString::number(...));
```

### Changer la formule de Fitts
Modifier la ligne dans `calculateResultHome()` :
```cpp
double value = (this->fittsModel->a * 1000) + (this->fittsModel->b * 1000 * F);
```

### Modifier la taille du cercle de démarrage
Dans `initGame()`, changer `int size = 180;`

### Ajouter un nouveau scénario Keystroke
Créer un nouveau slot similaire à `startKeystrokeEval()` avec un modèle de fichiers différent.

### Exporter les résultats en CSV
Ajouter une méthode qui itère sur `fittsModel->times`, `cercleCenter`, `cercleSize` et écrit dans un fichier CSV.

## Dépendances
- `QChart`, `QLineSeries`, `QScatterSeries` : Graphiques QtCharts
- `QElapsedTimer` : Chronométrage
- `QJsonDocument`, `QJsonArray` : Sérialisation JSON
- `QRandomGenerator` : Génération aléatoire des cibles
- `QGraphicsDropShadowEffect`, `QPropertyAnimation` : Effets visuels
- `QDesktopServices` : Ouverture de fichiers externes
- `QFileSystemModel` : Arborescence Keystroke

## Points d'attention
- **Fuite mémoire** : `keystrokeModel` n'est pas `delete` dans le destructeur (ligne commentée)
- **Fuite mémoire** : `timer` est `new` à chaque clic initial et à chaque `startKeystrokeEval()` sans `delete`
- **Pas de validation** : Si `minSize > maxSize`, le calcul de taille aléatoire peut buguer
- **Les graphiques sont recréés** à chaque changement de paramètre a/b — pas de mise à jour incrémentale
- **`mapToScene`** est utilisé pour convertir les coordonnées souris → scène, mais seulement après le premier clic
