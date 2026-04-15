# Documentation Développeur - FittsModel

## Localisation
`/fitts/model/fittsmodel.h` et `/fitts/model/fittsmodel.cpp`

## Rôle du fichier
C'est le **modèle de données** pour le test de la loi de Fitts. Il stocke tous les paramètres du test, les données collectées pendant l'expérience, et les statistiques calculées. Il gère aussi la sérialisation en JSON.

## Attributs

### Paramètres du test

| Attribut | Type | Valeur par défaut | Rôle |
|---|---|---|---|
| `a` | `double` | `0.20` | Constante empirique `a` de la loi de Fitts |
| `b` | `double` | `0.10` | Constante empirique `b` de la loi de Fitts |
| `nbCible` | `int` | `10` | Nombre de cibles à afficher pendant le test |
| `minSize` | `int` | `10` | Taille minimum des cibles (en pixels) |
| `maxSize` | `int` | `160` | Taille maximum des cibles (en pixels) |
| `cibleLeft` | `int` | `0` | Compteur de cibles restantes pendant le test |

### Données collectées

| Attribut | Type | Rôle |
|---|---|---|
| `clickPoints` | `QList<QPoint>` | Positions (x, y) de chaque clic de l'utilisateur |
| `cercleCenter` | `QList<QPoint>` | Positions (x, y) du centre de chaque cible |
| `cercleSize` | `QList<int>` | Diamètre de chaque cible |
| `times` | `QList<qint64>` | Temps (en ms) entre chaque clic successif |

### Statistiques calculées

| Attribut | Type | Rôle |
|---|---|---|
| `diffMoy` | `double` | Différence moyenne entre temps théorique et expérimental |
| `avgTime` | `double` | Temps moyen observé par cible |
| `ecartType` | `double` | Écart type (non utilisé actuellement) |
| `erreurType` | `double` | Erreur type (non utilisé actuellement) |
| `itc95` | `double` | Intervalle de confiance à 95% (non utilisé actuellement) |

## Fonctions

### `FittsModel()`
Constructeur par défaut. Les attributs sont initialisés directement dans le header.

### `writeDataJson()`
Sérialise l'ensemble des données du test en un `QJsonObject` :

```json
{
  "cibleLeft": 0,
  "nbCible": 10,
  "minSize": 10,
  "maxSize": 160,
  "a": 0.20,
  "b": 0.10,
  "ecartType": 0,
  "erreurType": 0,
  "diffMoy": 123.45,
  "itc95": 0,
  "avgTime": 456.78,
  "clickPoints": [{"x": 100, "y": 200}, ...],
  "cercleCenter": [{"x": 150, "y": 250}, ...],
  "cercleSize": [80, 120, ...],
  "dateTime": "01/04/2026 - 14:30"
}
```

**Note :** La date est générée via `QDateTime::currentDateTime()`. Il y a une fuite mémoire car `currentDate` est alloué avec `new` mais jamais `delete`.

### `QPointToQJsonobject(QPoint point)`
Utilitaire qui convertit un `QPoint` en `QJsonObject` avec les clés `"x"` et `"y"`.

```cpp
QJsonObject converter;
converter.insert("x", point.x());
converter.insert("y", point.y());
```

## Accès aux attributs

Tous les attributs sont **privés**. L'accès se fait via les déclarations `friend` :
```cpp
friend FittsView;       // La vue peut lire les données pour l'affichage
friend FittsController;  // Le contrôleur peut lire et écrire les données
```

Il n'y a **pas de getters/setters**. Le contrôleur et la vue accèdent directement aux attributs.

## Ce qu'on peut modifier

### Ajouter un nouveau paramètre
1. Déclarer l'attribut dans le `.h` :
```cpp
int newParam = 42;
```
2. L'ajouter à `writeDataJson()` :
```cpp
dataItem["newParam"] = newParam;
```

### Activer les statistiques commentées
Les attributs `ecartType`, `erreurType` et `itc95` existent déjà mais ne sont jamais calculés. Ajouter le calcul dans `FittsController::calculateResultHome()`.

### Ajouter des getters/setters
Pour un meilleur encapsulage, remplacer les `friend` par des accesseurs :
```cpp
public:
    double getA() const { return a; }
    void setA(double val) { a = val; }
```

### Sauvegarder les temps dans le JSON
Les temps (`times`) ne sont actuellement **pas** sérialisés dans `writeDataJson()`. Pour les ajouter :
```cpp
QJsonArray timesArray;
for (int i = 0; i < times.size(); i++) {
    timesArray.append(times[i]);
}
dataItem["times"] = timesArray;
```

## Dépendances
- `<QJsonArray>`, `<QJsonDocument>`, `<QJsonObject>` : Sérialisation JSON
- `<QList>`, `<QPoint>` : Structures de données

## Points d'attention
- **Fuite mémoire** dans `writeDataJson()` : `QDateTime *currentDate = new QDateTime;` sans `delete`. Utiliser directement `QDateTime::currentDateTime()` à la place.
- **Pas de sérialisation des temps** : La liste `times` n'est pas exportée en JSON
- **Pas de validation** : Aucune vérification que `minSize < maxSize` ou que `nbCible > 0`
- **`friend` classes** : Casse l'encapsulation — tout le contrôleur et la vue ont accès à tout
