# Dossier `res/` - Les Ressources de l'Application

## Métaphore Simple

Le dossier **`res/`** (resources = ressources) c'est comme le **garde-manger et la décoration du restaurant** :
- Les assiettes et couverts (icônes)
- Les affiches murales (images)
- La police d'écriture du menu (fonts)
- L'uniforme des serveurs (style visuel)

Ce sont tous les **fichiers non-code** que l'application utilise.

## Structure du Dossier

```
res/
├── app.ico              ← Icône de l'application (barre des tâches)
├── fonts/               ← Polices d'écriture
├── icons/               ← Petites images pour les boutons
├── img/                 ← Grandes images (logo, illustrations)
└── style/               ← Fichiers de style additionnels
```

## Contenu Détaillé

### `app.ico` - L'Icône de l'Application

- **Taille** : 51 241 octets
- **Format** : ICO (format Windows pour les icônes)
- **Utilisation** : Affichée dans :
  - La barre des tâches Windows
  - Le coin de la fenêtre
  - Le fichier .exe compilé
  - Le raccourci sur le bureau

### `fonts/` - Les Polices d'Écriture (6 items)

Ces fichiers sont au format **TTF** (TrueType Font) :

| Fichier | Style |
|---------|-------|
| `Roboto-Regular.ttf` | Normal |
| `Roboto-Italic.ttf` | Italique |
| `Roboto-Medium.ttf` | Gras moyen |
| `Roboto-MediumItalic.ttf` | Gras moyen + italique |
| `Roboto-Bold.ttf` | Gras |
| `Roboto-BoldItalic.ttf` | Gras + italique |

**Pourquoi Roboto ?**
- Police moderne créée par Google
- Très lisible à l'écran
- Look professionnel et épuré
- Gratuite et libre de droits

**Comment elles sont chargées ?**
```cpp
// Dans main.cpp
QFontDatabase::addApplicationFont(":/fonts/res/fonts/Roboto-Regular.ttf");
```

Le `:/` signifie que la police est embarquée dans l'exécutable via le système de ressources Qt.

### `icons/` - Les Icônes de Boutons (8 items)

Ce sont des fichiers **SVG** (Scalable Vector Graphics) ou **PNG** :
- Taille petite (16x16, 24x24, 32x32 pixels)
- Utilisées dans les boutons et la barre d'outils
- Exemples probables :
  - `play.svg` → Démarrer un test
  - `settings.svg` → Ouvrir les paramètres
  - `chart.svg` → Voir les graphiques
  - `save.svg` → Sauvegarder
  - `help.svg` → Aide
  - `home.svg` → Retour accueil
  - `theme-dark.svg` / `theme-light.svg` → Changer de thème

**Pourquoi SVG ?**
- Vecteur = redimensionnable sans perte de qualité
- Parfait pour les écrans Haute Définition (Retina/4K)
- Fichiers légers

### `img/` - Les Images (3 items)

Images de plus grande taille :
- Probablement le **logo** de l'application
- Des **illustrations** pour l'aide/tutorial
- Des **captures d'écran** d'exemple

Format probable : PNG ou JPG

### `style/` - Styles Additionnels (1 item)

Contient probablement :
- Des variations de thèmes
- Des styles pour composants spécifiques
- Des fichiers QSS additionnels

## Le Fichier `qtresources.qrc`

Ce fichier (à la racine du projet) est **la carte d'identité** des ressources :

```xml
<!DOCTYPE RCC>
<RCC version="1.0">
    <qresource prefix="/">
        <file>res/app.ico</file>
        <file>res/fonts/Roboto-Regular.ttf</file>
        ...
    </qresource>
</RCC>
```

**C'est quoi ce fichier ?**
- C'est un fichier XML qui liste toutes les ressources
- Il permet d'**embarquer** les fichiers dans l'exécutable final
- Plus besoin de dossier `res/` à côté du .exe !

**Analogie** : C'est comme faire une valise avec tout dedans. Au lieu de transporter 20 sacs séparés, tout est dans une seule mallette (l'exécutable).

## Pourquoi Embarquer les Ressources ?

| Avec embarquement | Sans embarquement |
|-------------------|-------------------|
| Un seul fichier .exe | Dossier `res/` + .exe |
| Pas de fichiers perdus | Risque de fichiers manquants |
| Chargement plus rapide | Chargement depuis le disque |
| Distribution simple | Plus compliqué à installer |

## Comment Accéder aux Ressources en Code ?

```cpp
// Accéder une police
QFontDatabase::addApplicationFont(":/fonts/res/fonts/Roboto-Bold.ttf");

// Accéder une icône
QIcon icon(":/icons/res/icons/play.svg");
button->setIcon(icon);

// Accéder une image
QPixmap image(":/img/res/img/logo.png");
label->setPixmap(image);
```

Le préfixe `:/` dit à Qt : "Cherche dans les ressources embarquées, pas sur le disque".

## Fichiers Complémentaires à la Racine

### `dark.qss` et `light.qss`

Ces fichiers sont **à la racine** du projet, pas dans `res/`, mais sont liés :

**`dark.qss`** (1 876 octets) : Thème sombre
- Fond gris foncé
- Texte blanc/gris clair
- Boutons turquoises
- Pour travailler le soir / moins fatiguant pour les yeux

**`light.qss`** (270 octets) : Thème clair
- Fond blanc
- Texte noir
- Style minimal (utilise les valeurs par défaut Qt)

**QSS c'est quoi ?**
- Qt StyleSheets
- Similaire au CSS des sites web
- Définit couleurs, polices, bordures, marges

Exemple de syntaxe QSS :
```css
QPushButton {
    background-color: #00DAC7;  /* Turquoise */
    color: white;
    border-radius: 4px;
}
```

## Résumé Visuel

```
[qtresources.qrc] ── liste ──► [app.ico]
                          ──► [fonts/*.ttf] ──► Texte de l'interface
                          ──► [icons/*.svg] ──► Boutons
                          ──► [img/*.png]   ──► Illustrations
                          ──► [style/*]     ──► Thèmes additionnels

Compilateur Qt ── embarque tout ──► [Fitts.exe]
                                        ↑
                                   Tout est dedans !
                                   (pas de dépendances externes)
```

## Conseil pour Modifier

Si tu veux changer l'apparence :
1. **Changer l'icône** → Remplace `app.ico` (même nom !)
2. **Changer la police** → Remplace les fichiers dans `fonts/`
3. **Changer les icônes** → Remplace les SVG dans `icons/`
4. **Changer les couleurs** → Modifie `dark.qss` ou `light.qss`

**Important** : Après modification, relancer qmake pour regénérer les ressources !
