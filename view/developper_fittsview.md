# Documentation Développeur - FittsView

## Localisation
`/fitts/view/fittsview.h` et `/fitts/view/fittsview.cpp`

## Rôle du fichier
C'est la **vue principale** de l'application (pattern MVC). Elle hérite de `QMainWindow` et gère l'ensemble de l'interface graphique : 8 écrans empilés via un `QStackedLayout`, une barre de menus, les graphiques, et le système de thèmes.

## Architecture de l'interface

L'application utilise un `QStackedLayout` pour empiler les écrans. Un seul est visible à la fois :

```
mainStack (QStackedLayout)
├── homeScreenWidget          → Écran d'accueil (choix Fitts / Keystroke)
├── settingsScreenWidget      → Paramètres du test Fitts
├── resultsScreenWidget       → Résultats du test Fitts
├── evalScreenWidget          → Zone de test Fitts (cibles)
├── keystrokeScreenWidget     → Accueil Keystroke (liste scénarios)
├── keystrokeSettingsScreenWidget → Paramètres Keystroke (K, P, H, M)
├── keystrokeEvalScreenWidget → Test Keystroke (arborescence)
└── keystrokeResultScreenWidget → Résultats Keystroke
```

## Attributs principaux

### Références MVC

| Attribut | Type | Rôle |
|---|---|---|
| `fittsModel` | `FittsModel*` | Accès en lecture au modèle |
| `fittsController` | `FittsController*` | Passé au constructeur (non stocké en attribut) |

### Widgets d'écrans

| Attribut | Type | Rôle |
|---|---|---|
| `mainStack` | `QStackedLayout*` | Layout empilé contenant tous les écrans |
| `homeScreenWidget` | `QWidget*` | Écran d'accueil |
| `evalScreenWidget` | `QWidget*` | Écran de test Fitts |
| `settingsScreenWidget` | `QWidget*` | Écran paramètres Fitts |
| `resultsScreenWidget` | `QWidget*` | Écran résultats Fitts |
| `keystrokeScreenWidget` | `QWidget*` | Écran accueil Keystroke |
| `keystrokeSettingsScreenWidget` | `QWidget*` | Écran paramètres Keystroke |
| `keystrokeEvalScreenWidget` | `QWidget*` | Écran test Keystroke |
| `keystrokeResultScreenWidget` | `QWidget*` | Écran résultats Keystroke |

### Zone de test Fitts

| Attribut | Type | Rôle |
|---|---|---|
| `graphicView` | `GraphicWidget*` | Vue graphique qui capte les clics |
| `scene` | `QGraphicsScene*` | Scène où sont dessinées les cibles |

### Inputs Fitts

| Attribut | Type | Paramètre |
|---|---|---|
| `aValue` | `QDoubleSpinBox*` | Constante `a` |
| `bValue` | `QDoubleSpinBox*` | Constante `b` |
| `nbCible` | `QSpinBox*` | Nombre de cibles |
| `minSize` | `QSpinBox*` | Taille min des cibles |
| `maxSize` | `QSpinBox*` | Taille max des cibles |

### Inputs Keystroke

| Attribut | Type | Paramètre |
|---|---|---|
| `inputK` | `QSpinBox*` | Durée K (frappe/clic) |
| `inputP` | `QSpinBox*` | Durée P (déplacement souris) |
| `inputH` | `QSpinBox*` | Durée H (changement souris-clavier) |
| `inputM` | `QSpinBox*` | Durée M (réflexion mentale) |

### Labels de résultats

| Attribut | Type | Contenu |
|---|---|---|
| `testLabel` | `QLabel*` | "X cibles restantes" pendant le test |
| `diffMoy` | `QLabel*` | Différence moyenne (résultats) |
| `avgTime` | `QLabel*` | Temps moyen (résultats) |
| `keystrokeResultLabel` | `QLabel*` | Résultats du test Keystroke |

### Graphiques

| Attribut | Type | Rôle |
|---|---|---|
| `plotHome` | `QChartView*` | Graphique "Temps par cible" |
| `plotHomeDistance` | `QChartView*` | Graphique "Temps vs distance" |

## Fonctions

### `FittsView(FittsController*, FittsModel*)`
Constructeur :
1. Redimensionne la fenêtre à 640×480
2. Appelle `initWindows()` pour construire l'UI
3. Connecte tous les signaux/slots entre la vue et le contrôleur

**Connexions signal/slot :**

| Signal (Vue) | Slot (Contrôleur) |
|---|---|
| `buttonStartFitts::clicked` | `initEvalScreen()` |
| `startBtn::clicked` | `initEvalScreen()` |
| `btnBackFromResults::clicked` | `initEvalScreen()` |
| `btnFittsSettings::clicked` | `initSettingsScreen()` |
| `btnGoToJson::clicked` | `openJsonFile()` |
| `actionTutorial::triggered` | `showHelp()` |
| `btnSaveKeystrokeSettings::clicked` | `saveKeystrokeSettings()` |
| `btnKeystrokeSettings::clicked` | `loadKeystrokeSettings()` |
| `defaultScenarioBtn::clicked` | `startKeystrokeEval()` |
| `tree::doubleClicked` | `endKeystrokeEval()` |
| `graphicView::mouseClicked` | `cibleClicked()` |
| `aValue::valueChanged` | `aValueChanged()` |
| `bValue::valueChanged` | `bValueChanged()` |
| `nbCible::valueChanged` | `nbCibleChanged()` |
| `minSize::valueChanged` | `minSizeChanged()` |
| `maxSize::valueChanged` | `maxSizeChanged()` |

### `initWindows()`
Construit l'intégralité de l'interface. C'est la fonction la plus longue du projet (~640 lignes). Elle crée dans l'ordre :

1. **Style global** : charge `:/style/global` depuis les ressources
2. **Barre de menus** : Fichier (Quitter), Affichage (Thème sombre/clair), Aide (F1)
3. **Écran d'accueil** : deux `QToolButton` avec icônes (Fitts et Keystroke)
4. **Écran résultats** : labels de stats, spinboxes a/b/nbCible, légende, graphiques dans un `QTabWidget`
5. **Écran paramètres** : formule (image), taille min/max avec `QSpinBox`
6. **Écran de test** : `GraphicWidget` + scène + boutons retour/réglages
7. **Écrans Keystroke** : accueil scénarios, paramètres K/P/H/M, arborescence fichiers, résultats

### `updateTestMsg()`
Met à jour le label pendant le test :
```html
<span style='font-size: 64px; font-weight: 500'>X</span> cibles restantes
```

### `changeTheme(QAction* selectedAction)`
Change le thème de l'application :
- Thème sombre → charge `:/style/dark.qss`
- Thème clair → charge `:/style/light.qss`
- Applique le stylesheet via `qApp->setStyleSheet()`

Le thème sombre est activé par défaut au démarrage.

## Ce qu'on peut modifier

### Changer la taille de fenêtre par défaut
```cpp
this->resize(800, 600); // Ligne 14 de fittsview.cpp
```

### Ajouter un nouvel écran
1. Déclarer le widget dans le `.h` :
```cpp
QWidget *newScreenWidget;
```
2. Le créer et l'ajouter au stack dans `initWindows()` :
```cpp
newScreenWidget = new QWidget;
mainStack->addWidget(newScreenWidget);
```
3. Naviguer vers cet écran :
```cpp
mainStack->setCurrentWidget(newScreenWidget);
```

### Ajouter un nouveau graphique
```cpp
QChartView *newPlot = new QChartView;
newPlot->setRenderHint(QPainter::Antialiasing);
graphSelector->addTab(newPlot, "Nouveau graphique");
```

### Modifier les bornes des spinboxes
Les valeurs sont définies dans `initWindows()` :
- `nbCible` : min 5, max 100
- `minSize` : min 10, max 150
- `maxSize` : min 160, max 400
- Paramètres Keystroke : min 0, max 10000, pas de 100

### Ajouter un troisième thème
1. Créer un fichier `.qss` dans les ressources
2. Ajouter une `QAction` dans le menu thème
3. Ajouter le cas dans `changeTheme()`

## Dépendances
- `<QMainWindow>` : Fenêtre principale avec barre de menus
- `<QStackedLayout>` : Navigation entre écrans
- `<QGraphicsView>`, `<QGraphicsScene>` : Zone de test Fitts
- `<QtCharts>` : Graphiques de résultats
- `<QTreeView>` : Arborescence fichiers Keystroke
- `graphicwidget.h` : Widget personnalisé pour les clics

## Points d'attention
- **`initWindows()` est très longue** (~640 lignes) : une refactorisation en sous-méthodes (un par écran) améliorerait la lisibilité
- **Paramètres a et b déplacés** : Ils ne sont plus dans l'écran de paramètres mais dans l'écran de résultats (le code commenté montre l'ancienne position)
- **`friend FittsController`** : Le contrôleur accède directement aux widgets privés de la vue
- **La scène est de taille fixe** : `scene->setSceneRect(QRect(0, 0, graphicView->width() - 64, 256))` — pas de redimensionnement dynamique
- **Les labels `ecartType`, `erreurType`, `itc95`** sont déclarés dans le header mais leur création est commentée dans `initWindows()`
