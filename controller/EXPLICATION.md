# Dossier `controller/` - Le Cerveau de l'Application

## Métaphore Simple

Imagine le **Controller** comme le **serveur d'un restaurant** :
- Il écoute ce que veut le client (tes clics)
- Il transmet à la cuisine (le Model pour stocker)
- Il rapporte l'assiette au client (la View pour afficher)

## Fichiers dans ce Dossier

### `fittscontroller.h` - Le Plan (Header)

C'est comme le **menu du restaurant** avec la liste de tout ce qu'on peut faire :

```cpp
class FittsController : public QObject
```
→ "Je suis le chef, j'hérite des capacités de base Qt"

#### Les Méthodes Publiques (actions visibles)

| Méthode | Ce qu'elle fait (en simple) |
|---------|----------------------------|
| `FittsController()` | Le constructeur : allume le cerveau |
| `~FittsController()` | Le destructeur : éteint proprement |
| `getHisto()` | Donne l'historique des tests passés |

#### Les Méthodes Privées (actions internes)

| Méthode | Explication simple |
|---------|------------------|
| `displayResults()` | Montre les résultats à la fin |
| `initGame()` | Prépare un nouveau test |
| `finish()` | Termine le test en cours |
| `nextCible()` | Passe à la cible suivante |
| `calculateResultHome()` | Fait les maths pour les graphiques |
| `addHisto()` | Sauvegarde dans le fichier JSON |

#### Les "Slots" (réactions aux événements)

Les **slots** sont comme des **boutons d'urgence** avec des étiquettes :
- Quelqu'un clique quelque part → le slot correspondant se déclenche

**Slots publics** (tout le monde peut déclencher) :
- `showHelp()` → Ouvre l'aide/tutorial

**Slots privés** (déclenchés par le code lui-même) :
- `initEvalScreen()` → Prépare l'écran de test
- `initResultsScreen()` → Prépare l'écran de résultats
- `initSettingsScreen()` → Prépare l'écran de paramètres
- `openJsonFile()` → Ouvre le fichier de sauvegarde

**Slots pour les paramètres** (quand tu changes une valeur) :
- `aValueChanged(double)` → Tu as changé le paramètre "a"
- `bValueChanged(double)` → Tu as changé le paramètre "b"
- `nbCibleChanged(int)` → Tu as changé le nombre de cibles
- `minSizeChanged(int)` → Tu as changé la taille minimum
- `maxSizeChanged(int)` → Tu as changé la taille maximum

**Slots pour les clics** :
- `cibleClicked(int x, int y)` → Tu as cliqué sur une cible aux coordonnées x,y

**Slots pour Keystroke** :
- `saveKeystrokeSettings()` → Sauvegarde les réglages Keystroke
- `loadKeystrokeSettings()` → Charge les réglages Keystroke
- `startKeystrokeEval()` → Démarre le test Keystroke
- `endKeystrokeEval(index)` → Termine quand tu choisis une ligne

#### Les Données Stockées

```cpp
QList<FittsModel> *histModel;     // L'historique de tous les tests
FittsModel *fittsModel;           // Le test actuel
FittsView *fittsView;             // L'écran actuel
KeystrokeModel *keystrokeModel;    // Le modèle Keystroke
QElapsedTimer *timer;             // Le chronomètre
```

### `fittscontroller.cpp` - Les Actions Réelles

Si `.h` est le menu, `.cpp` est la **recette détaillée**.

Ce fichier contient le code réel qui :
1. **Calcule la Loi de Fitts** avec la formule mathématique
2. **Crée les graphiques** avec Qt Charts
3. **Gère le chronomètre** entre chaque clic
4. **Sauvegarde en JSON** pour l'historique

## Concepts Clés Expliqués

### `#include` - Les Ingrédients Importés

Au début du fichier `.h`, tu vois plein de `#include` :
```cpp
#include <QApplication>    // Pour l'application de base
#include <QChart>          // Pour faire des graphiques
#include <QElapsedTimer>   // Pour chronométrer
#include <QJsonObject>     // Pour sauvegarder
```

C'est comme dire : "Pour cette recette, j'ai besoin d'œufs, de farine, de sucre..."

### `Q_OBJECT` - La Magie Qt

```cpp
Q_OBJECT
```
C'est une **étiquette spéciale Qt** qui permet d'utiliser :
- Les signaux/slots (communication entre objets)
- La introspection (l'objet peut se décrire lui-même)

### Pointeurs (`*`)

```cpp
FittsModel *fittsModel;
```
Le `*` signifie que ce n'est pas l'objet lui-même, mais son **adresse**.
Comme avoir l'adresse d'une maison plutôt que la maison en entier.

Pourquoi ? Pour économiser la mémoire et partager le même objet entre plusieurs endroits.

### `friend` - Les Amis Intimes

```cpp
friend FittsView;
friend FittsController;
```

Dans le Model, ça signifie : "Ces classes peuvent voir mes secrets (mes données privées)".
C'est une relation d'intimité dans le code.

## Résumé Visuel

```
[CLIC UTILISATEUR]
       ↓
[fittscontroller.cpp reçoit l'événement]
       ↓
[Met à jour fittsmodel (les données)]
       ↓
[Demande à fittsview d'afficher]
       ↓
[ÉCRAN MIS À JOUR]
```
