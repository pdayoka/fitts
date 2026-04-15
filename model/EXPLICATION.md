# Dossier `model/` - La Mémoire de l'Application

## Métaphore Simple

Le **Model** c'est comme le **cahier de notes** du restaurant :
- Il note tout ce qui se passe
- Il garde les recettes (paramètres)
- Il stocke les commandes passées (historique)
- Il ne décide rien, il se contente de mémoriser

## Fichiers dans ce Dossier

### `fittsmodel.h` - Le Plan des Données Fitts

```cpp
class FittsModel
```
→ "Je suis le carnet qui mémorise tout pour la Loi de Fitts"

#### Les Paramètres du Test (Configuration)

| Variable | Valeur défaut | C'est quoi ? |
|----------|--------------|--------------|
| `cibleLeft` | 0 | Combien de cibles restent à cliquer |
| `nbCible` | 10 | Nombre total de cibles dans le test |
| `minSize` | 10 | Taille minimum d'une cible (pixels) |
| `maxSize` | 160 | Taille maximum d'une cible (pixels) |
| `a` | 0.20 | Paramètre "a" de la formule Fitts |
| `b` | 0.10 | Paramètre "b" de la formule Fitts |

**Les paramètres a et b expliqués :**
- La formule Fitts est : **T = a + b × log₂(Distance/Taille + 1)**
- `a` = temps de base (intercept) - le temps minimum pour réagir
- `b` = coefficient (pente) - à quel point la difficulté augmente le temps
- Plus `a` est grand = plus tu mets de temps à démarrer
- Plus `b` est grand = plus la distance/taille compte

#### Les Résultats Calculés (Stats)

| Variable | Type | Explication |
|----------|------|-------------|
| `ecartType` | double | Écart-type = mesure de dispersion des temps |
| `erreurType` | double | Erreur type = précision de la moyenne |
| `diffMoy` | double | Différence moyenne (théorie vs réalité) |
| `itc95` | double | Intervalle de confiance à 95% |
| `avgTime` | double | Temps moyen par cible |

**Explication simple des stats :**
- **Écart-type** : Tes temps varient beaucoup ou pas ? (0 = tous identiques)
- **Erreur type** : La moyenne est-elle fiable ?
- **Différence moyenne** : La formule Fitts prédit bien la réalité ?
- **Intervalle 95%** : Zone où on est sûr à 95% de trouver la vraie valeur

#### Les Listes de Données Brutes

```cpp
QList<QPoint> clickPoints;    // Où tu as cliqué (x,y)
QList<QPoint> cercleCenter;   // Où étaient les cercles (x,y)
QList<int> cercleSize;        // Quelle taille avait chaque cercle
QList<qint64> times;          // Combien de temps pour chaque clic (ms)
```

**Analogie :** C'est comme avoir :
- Des photos de où tu as mis ta flèche (clickPoints)
- Des photos de où était la cible (cercleCenter)
- La taille de chaque cible (cercleSize)
- Le temps pris pour chaque tir (times)

### `fittsmodel.cpp` - Les Actions sur les Données

Ce fichier contient :
1. **Le constructeur** : initialise un carnet vierge
2. **`writeDataJson()`** : convertit tout en format JSON pour sauvegarder
3. **`QPointToQJsonobject()`** : convertit une position (x,y) en format sauvegardable

**JSON c'est quoi ?** C'est un format de texte structuré :
```json
{
  "nbCible": 10,
  "a": 0.20,
  "times": [450, 520, 480, ...]
}
```
Parfait pour sauvegarder et relire plus tard.

### `keystrokemodel.h` - Le Plan pour Keystroke

```cpp
class KeystrokeModel
```
→ "Je mémorise les paramètres du modèle Keystroke"

Ce fichier est petit (391 octets) car le modèle Keystroke est plus simple :
- Stocke les paramètres K, P, H, M
- Pas besoin de listes complexes comme Fitts

### `keystrokemodel.cpp` - Les Actions Keystroke

Très minimal (68 octets) - juste le constructeur pour initialiser les valeurs.

## Explication des Types de Données

| Type | C'est quoi ? | Exemple |
|------|-------------|---------|
| `int` | Nombre entier | 10, -5, 0 |
| `double` | Nombre décimal | 0.20, 3.14159 |
| `qint64` | Grand nombre entier (64 bits) | 450000 millisecondes |
| `QPoint` | Point avec x et y | (100, 200) sur l'écran |
| `QList<T>` | Liste de choses | [item1, item2, item3] |

## Le Mot Magique `friend`

```cpp
friend FittsView;
friend FittsController;
```

Normalement, les données `private` sont secrètes. Mais ici :
- **FittsView** peut lire pour afficher
- **FittsController** peut lire et écrire pour gérer le test

C'est comme donner la clé de ton cahier à ton professeur (Controller) et à l'afficheur (View).

## Schéma de Fonctionnement

```
[DÉBUT DU TEST]
       ↓
[Controller crée un FittsModel vide]
       ↓
[À chaque clic : Controller écrit dans le Model]
  - Ajoute le temps à `times`
  - Ajoute la position à `clickPoints`
  - Ajoute la taille à `cercleSize`
       ↓
[FIN DU TEST]
       ↓
[Controller lit tout et calcule les stats]
       ↓
[Sauvegarde en JSON via writeDataJson()]
```

## Pourquoi séparer le Model ?

Avantages de cette architecture :
1. **Répétable** : On peut faire 100 tests, chacun a son Model
2. **Sauvegardable** : Facile de tout mettre dans un fichier
3. **Testable** : On peut vérifier les calculs séparément
4. **Modulaire** : On pourrait changer l'interface sans toucher aux données
