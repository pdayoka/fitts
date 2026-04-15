# Modifications possibles - main.cpp

## Localisation
`/fitts/main.cpp`

---

## 1. Changer le titre de l'application

### Avant
```cpp
a.setApplicationName("Music Download");
a.setApplicationDisplayName("Music Download");
```

### Après
```cpp
a.setApplicationName("Fitts Experiment");
a.setApplicationDisplayName("Fitts Experiment - IHM");
```

### Résultat
Le titre affiché dans la barre de titre de la fenêtre et dans la barre des tâches change.

---

## 2. Désactiver le scaling haute DPI

### Avant
```cpp
QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
```

### Après
```cpp
// Ligne supprimée ou commentée
// QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
```

### Résultat
Sur les écrans 4K/Retina, l'interface apparaîtra très petite (non mise à l'échelle). Utile si vous voulez contrôler manuellement la taille.

---

## 3. Ajouter un splash screen au démarrage

### Avant
```cpp
FittsController *controller = new FittsController;
int exitCode = a.exec();
```

### Après
```cpp
QSplashScreen splash(QPixmap(":/img/splash.png"));
splash.show();
a.processEvents();

FittsController *controller = new FittsController;
splash.finish(controller->fittsView); // Se ferme quand la fenêtre principale apparaît

int exitCode = a.exec();
```

### Résultat
Un écran d'accueil avec une image s'affiche pendant le chargement de l'application. Il disparaît automatiquement quand la fenêtre principale est prête. Il faut ajouter l'image `splash.png` dans les ressources Qt.

---

## 4. Forcer le style Fusion

### Avant
```cpp
QApplication a(argc, argv);
```

### Après
```cpp
QApplication a(argc, argv);
QApplication::setStyle(QStyleFactory::create("Fusion"));
```

### Résultat
L'application utilise le style "Fusion" de Qt (apparence uniforme sur toutes les plateformes). Les thèmes sombre/clair de l'application seront appliqués par-dessus ce style.

---

## 5. Ajouter une police personnalisée

### Avant
```cpp
QFontDatabase::addApplicationFont(":/fonts/Roboto-Bold");
QFontDatabase::addApplicationFont(":/fonts/Roboto-BoldItalic");
// ... etc
```

### Après
```cpp
QFontDatabase::addApplicationFont(":/fonts/Roboto-Bold");
QFontDatabase::addApplicationFont(":/fonts/Roboto-BoldItalic");
// ... polices existantes ...
QFontDatabase::addApplicationFont(":/fonts/OpenSans-Regular");

// Pour l'utiliser globalement :
QFont defaultFont("Open Sans", 10);
a.setFont(defaultFont);
```

### Résultat
La police Open Sans est chargée et définie comme police par défaut de l'application. Il faut ajouter le fichier `.ttf` dans les ressources Qt (`qtresources.qrc`).

---

## 6. Définir une taille de fenêtre fixe

### Avant
```cpp
FittsController *controller = new FittsController;
```

### Après
```cpp
FittsController *controller = new FittsController;
// Accès à la vue via le contrôleur (nécessite que fittsView soit public)
// Sinon modifier dans fittsview.cpp directement
```

### Résultat
Voir `modification_fittsview.md` pour modifier la taille de fenêtre (c'est dans `FittsView` que `resize()` est appelé).
