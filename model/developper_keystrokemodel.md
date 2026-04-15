# Documentation Développeur - KeystrokeModel

## Localisation
`/fitts/model/keystrokemodel.h` et `/fitts/model/keystrokemodel.cpp`

## Rôle du fichier
Modèle de données pour le **test Keystroke-Level Model (KLM)**. Il stocke les durées théoriques des opérateurs élémentaires utilisés pour prédire le temps d'exécution d'une tâche utilisateur.

## Le modèle KLM en bref

Le modèle Keystroke-Level (Card, Moran & Newell, 1983) décompose une tâche en opérateurs élémentaires :
- **K** (Keystroking) : appuyer sur une touche ou cliquer
- **P** (Pointing) : déplacer la souris vers une cible
- **H** (Homing) : basculer la main entre le clavier et la souris
- **M** (Mental) : temps de réflexion / préparation mentale

Le temps théorique d'une tâche = somme des durées de chaque opérateur.

## Attributs

| Attribut | Type | Valeur par défaut | Rôle |
|---|---|---|---|
| `k` | `int` | `200` ms | Durée d'une frappe clavier ou d'un clic souris |
| `p` | `int` | `1100` ms | Durée d'un déplacement de souris vers une cible |
| `h` | `int` | `400` ms | Durée d'un changement main souris ↔ clavier |
| `m` | `int` | `1300` ms | Durée d'une opération mentale (réflexion) |
| `lastMeasuredTime` | `int` | non initialisé | Dernier temps expérimental mesuré lors du test |

Tous les attributs sont **publics** — accès direct sans getters/setters.

## Fonctions

### `KeystrokeModel()`
Constructeur vide. Les valeurs par défaut sont définies directement dans le header.

## Utilisation dans l'application

Le contrôleur utilise ce modèle pour le scénario "Ouvrir un fichier dans une arborescence" :

**Encodage KLM de la tâche (méthode souris) :**
```
H (PK)^(2×profondeur) H
```

**Formule du temps théorique :**
```cpp
int theoreticalDuration = h + (p + k * 2) * depth + h;
```

Où `depth` = profondeur du fichier cliqué dans l'arborescence.

**Exemple :** Pour un fichier à profondeur 3 :
- `T = 400 + (1100 + 200×2) × 3 + 400 = 5300 ms`

## Ce qu'on peut modifier

### Modifier les valeurs par défaut
Changer directement les initialisations dans le header :
```cpp
int k = 280;  // Dactylo expérimenté : 280ms
int p = 1100; // Valeur standard
int h = 400;  // Valeur standard
int m = 1350; // Valeur standard
```

### Ajouter un nouvel opérateur
Par exemple l'opérateur **R** (Response time du système) :
```cpp
int r = 1000; // Temps de réponse système en ms
```

### Ajouter une sérialisation JSON
Comme pour `FittsModel`, ajouter une méthode d'export :
```cpp
QJsonObject writeDataJson() {
    QJsonObject obj;
    obj["k"] = k;
    obj["p"] = p;
    obj["h"] = h;
    obj["m"] = m;
    obj["lastMeasuredTime"] = lastMeasuredTime;
    return obj;
}
```

### Ajouter un historique des tests Keystroke
Actuellement, seul le dernier test est conservé via `lastMeasuredTime`. On pourrait stocker une liste :
```cpp
QList<int> measuredTimes;
QList<int> theoreticalTimes;
QList<int> depths;
```

## Dépendances
- `<QString>` : Inclus mais non utilisé dans le code actuel

## Points d'attention
- **`lastMeasuredTime` non initialisé** : Sa valeur est indéfinie avant le premier test. Initialiser à `0` serait plus sûr.
- **`m` non utilisé** : Le paramètre de charge mentale est stocké mais jamais utilisé dans le calcul théorique (`endKeystrokeEval` ne l'inclut pas). C'est voulu et documenté dans l'interface : "la charge mentale n'est pas prise en compte dans ce scénario".
- **Pas de persistance** : Contrairement à `FittsModel`, les résultats Keystroke ne sont pas sauvegardés en JSON.
