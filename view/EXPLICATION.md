# Dossier `view/` - Les Écrans de l'Application

## Métaphore Simple

Le **View** c'est comme la **décoration et l'agencement du restaurant** :
- Les tables et chaises (les boutons)
- Le menu affiché (les labels)
- Les couleurs des murs (le thème)
- Les tableaux noirs (les graphiques)

Le View **ne décide de rien**, il se contente d'**afficher** ce que le Controller lui demande.

## Fichiers dans ce Dossier

### `fittsview.h` - Le Plan de la Déco

```cpp
class FittsView : public QMainWindow
```
→ "Je suis la fenêtre principale qui contient tout l'affichage"

#### Les Méthodes Publiques

| Méthode | Rôle |
|---------|------|
| `FittsView(...)` | Constructeur : construit la fenêtre |
| `~FittsView()` | Destructeur : nettoie |
| `initWindows()` | Crée tous les boutons, labels, zones |

#### Les Méthodes Privées

| Méthode | Ce qu'elle fait |
|---------|----------------|
| `updateTestMsg()` | Change le message pendant le test |
| `displayHisto()` | Montre l'historique des tests passés |
| `changeTheme(...)` | Passe du sombre au clair |
| `initPalette()` | Prépare les couleurs |

#### Les Écrans (Widgets) - Les "Pièces" de l'App

```cpp
QStackedLayout *mainStack;           // Le système de changement d'écran

QWidget *homeScreenWidget;           // Écran d'accueil (choix Fitts/Keystroke)
QWidget *evalScreenWidget;           // Écran de test Fitts (les cercles)
QWidget *settingsScreenWidget;       // Écran des paramètres (sliders)
QWidget *resultsScreenWidget;        // Écran des résultats (graphiques)
QWidget *keystrokeScreenWidget;      // Écran principal Keystroke
QWidget *keystrokeSettingsScreenWidget;  // Paramètres Keystroke
QWidget *keystrokeEvalScreenWidget;     // Test Keystroke
QWidget *keystrokeResultScreenWidget;   // Résultats Keystroke
```

**Métaphore** : `QStackedLayout` c'est comme un **fait-tout** de cuisine avec plusieurs compartiments superposés :
- Tu vois uniquement celui du dessus
- Tu peux changer pour en montrer un autre
- Les autres sont cachés en dessous

#### Les Boutons et Contrôles

**Écran d'accueil :**
```cpp
QToolButton *buttonStartFitts;       // Gros bouton "Lancer Fitts"
QToolButton *buttonStartKeystroke;   // Gros bouton "Lancer Keystroke"
```

**Navigation :**
```cpp
QPushButton *startBtn;               // Bouton "Commencer le test"
QPushButton *btnFittsSettings;       // Bouton "Paramètres Fitts"
QPushButton *btnBackFromResults;     // Bouton "Retour"
QPushButton *btnGoToJson;            // Bouton "Voir l'historique"
```

**Keystroke :**
```cpp
QPushButton *btnSaveKeystrokeSettings;  // Sauvegarde les réglages
QPushButton *btnKeystrokeSettings;      // Va aux paramètres
QPushButton *defaultScenarioBtn;        // Charge scénario par défaut
```

#### Les Champs de Saisie (Inputs)

**Paramètres Fitts :**
```cpp
QDoubleSpinBox *aValue;    // Champ pour régler "a" (avec décimales)
QDoubleSpinBox *bValue;    // Champ pour régler "b" (avec décimales)
QSpinBox *nbCible;         // Champ pour nombre de cibles (entier)
QSpinBox *minSize;         // Champ taille minimum
QSpinBox *maxSize;         // Champ taille maximum
```

**Paramètres Keystroke :**
```cpp
QSpinBox *inputK;          // Temps par touche (ms)
QSpinBox *inputP;          // Temps de pointage (ms)
QSpinBox *inputH;          // Temps retour Home (ms)
QSpinBox *inputM;          // Temps préparation (ms)
```

**Différence :**
- `QSpinBox` = nombres entiers (10, 20, 30...)
- `QDoubleSpinBox` = nombres décimaux (0.20, 0.15...)

#### La Zone de Test Fitts (le "Terrain de Jeu")

```cpp
GraphicWidget *graphicView;   // La zone où apparaissent les cercles
QGraphicsScene *scene;       // La "scène" derrière qui gère les dessins
```

C'est comme une **toile blanche** où on dessine les cibles à cliquer.

#### Les Labels de Statistiques

```cpp
QLabel *testLabel;     // Message "Cible 3/10, Temps: 450ms"
QLabel *ecartType;     // Affiche l'écart-type calculé
QLabel *erreurType;    // Affiche l'erreur type
QLabel *diffMoy;       // Affiche la différence moyenne
QLabel *itc95;         // Affiche l'intervalle de confiance
QLabel *avgTime;       // Affiche le temps moyen
```

`QLabel` = étiquette de texte (non modifiable par l'utilisateur)

#### Les Graphiques

```cpp
QChartView *plotHome;           // Graphique : temps par cible
QChartView *plotHomeDistance;     // Graphique : temps vs distance
```

`QChartView` = une fenêtre dans la fenêtre qui affiche un graphique Qt Charts.

#### Les Thèmes (Palettes de Couleurs)

```cpp
QAction *actionDarkTheme;    // Action "Passer en mode sombre"
QAction *actionLightTheme;   // Action "Passer en mode clair"
QPalette darkPalette;        // Les couleurs du thème sombre
```

`QAction` = une action de menu (dans la barre du haut)
`QPalette` = un ensemble de couleurs (texte, fond, boutons...)

### `fittsview.cpp` - La Construction Réelle

Ce fichier (28 251 octets !) contient tout le code qui :
1. **Crée chaque bouton** un par un
2. **Positionne tout** avec des layouts (grilles, piles)
3. **Connecte les signaux** (quand tu cliques → appelle le Controller)
4. **Applique les styles** (couleurs, polices, tailles)
5. **Crée les graphiques** avec des séries de données

### `graphicwidget.h` et `.cpp` - Le Widget Spécial Dessin

```cpp
class GraphicWidget
```
→ "Je suis la zone spéciale qui dessine les cercles rouges/verts"

C'est une **sous-classe** personnalisée pour :
- Dessiner des cercles à des positions aléatoires
- Détecter les clics sur ces cercles
- Changer la couleur quand cliqué

## Les Couleurs Définies (en bas de fittsview.h)

```cpp
color_blue    = (0, 218, 199)     // Turquoise
color_purple  = (189, 136, 247)  // Violet
color_red     = (212, 102, 121)  // Rouge saumon
color_white   = (255, 255, 255)  // Blanc
color_grid    = (61, 63, 68)     // Gris foncé
```

Ces couleurs sont utilisées pour les graphiques et l'interface.

## Les `friend` - Les Amis du View

```cpp
friend FittsController;
```

Le Controller est "ami" du View, ce qui signifie :
- Le Controller peut accéder aux boutons, labels, etc.
- Le Controller peut demander d'afficher tel ou tel écran
- Le Controller peut lire les valeurs des champs de saisie

## Schéma Visuel du View

```
┌─────────────────────────────────────┐
│  [Barre de menu : Aide | Thème]     │
├─────────────────────────────────────┤
│                                     │
│  ┌───────────────────────────────┐  │
│  │   ECRAN ACTIF (mainStack)     │  │
│  │                               │  │
│  │  • Accueil (homeScreen)       │  │
│  │  • Test Fitts (evalScreen)    │  │  ←─ un seul visible à la fois
│  │  • Paramètres (settings)      │  │
│  │  • Résultats (results)        │  │
│  │  • etc.                       │  │
│  └───────────────────────────────┘  │
│                                     │
└─────────────────────────────────────┘
```

## Le Cycle de Vie du View

1. **Construction** : Le Controller crée le View
2. `initWindows()` : Crée tous les widgets (boutons, labels...)
3. `initPalette()` : Prépare les couleurs sombre/clair
4. **Affichage** : Montre l'écran d'accueil
5. **Interactions** : Attends les clics, informe le Controller
6. **Changements** : Le Controller demande de changer d'écran
7. **Destruction** : Nettoie tout à la fermeture
