# Modifications possibles - FittsView

## Localisation
`/fitts/view/fittsview.h` et `/fitts/view/fittsview.cpp`

---

## 1. Changer la taille de fenêtre par défaut

### Avant
```cpp
this->resize(640, 480);
```

### Après
```cpp
this->resize(1024, 768);
```

### Résultat
La fenêtre s'ouvre plus grande. La zone de test Fitts est aussi plus grande, ce qui donne plus d'espace pour placer les cibles. La fenêtre reste redimensionnable.

---

## 2. Forcer une fenêtre en plein écran

### Avant
```cpp
this->resize(640, 480);
this->initWindows();
```

### Après
```cpp
this->resize(640, 480);
this->initWindows();
this->showMaximized(); // Plein écran fenêtré
// ou : this->showFullScreen(); // Plein écran total
```

### Résultat
L'application s'ouvre directement en plein écran. Avec `showMaximized()` la barre de titre reste visible, avec `showFullScreen()` tout l'écran est occupé.

---

## 3. Changer le thème par défaut (clair au lieu de sombre)

### Avant
```cpp
actionDarkTheme->setChecked(true);
// ...
changeTheme(actionDarkTheme);
```

### Après
```cpp
actionLightTheme->setChecked(true);
// ...
changeTheme(actionLightTheme);
```

### Résultat
L'application démarre avec le thème clair au lieu du thème sombre. L'utilisateur peut toujours basculer via le menu Affichage > Thème.

---

## 4. Ajouter un troisième thème (ex: bleu)

### Avant (changeTheme)
```cpp
void FittsView::changeTheme(QAction* selectedAction) {
    QFile styleFile;
    if (selectedAction == actionDarkTheme)
        styleFile.setFileName(":/style/dark.qss");
    else
        styleFile.setFileName(":/style/light.qss");
    // ...
}
```

### Après
Dans `fittsview.h`, ajouter :
```cpp
QAction *actionBlueTheme;
```

Dans `initWindows()`, ajouter après les autres thèmes :
```cpp
actionBlueTheme = new QAction("Thème &bleu");
actionBlueTheme->setCheckable(true);
menuTheme->addAction(actionBlueTheme);
actionGroupTheme->addAction(actionBlueTheme);
```

Dans `changeTheme()` :
```cpp
void FittsView::changeTheme(QAction* selectedAction) {
    QFile styleFile;
    if (selectedAction == actionDarkTheme)
        styleFile.setFileName(":/style/dark.qss");
    else if (selectedAction == actionBlueTheme)
        styleFile.setFileName(":/style/blue.qss");
    else
        styleFile.setFileName(":/style/light.qss");

    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        qApp->setStyleSheet(styleSheet);
    }
}
```

### Résultat
Un troisième thème apparaît dans le menu. Il faut créer le fichier `blue.qss` et l'ajouter dans `qtresources.qrc`.

---

## 5. Modifier les bornes du nombre de cibles

### Avant
```cpp
nbCible = new QSpinBox;
nbCible->setValue(this->fittsModel->nbCible);
nbCible->setMaximum(100);
nbCible->setMinimum(5);
```

### Après
```cpp
nbCible = new QSpinBox;
nbCible->setValue(this->fittsModel->nbCible);
nbCible->setMaximum(50);   // Maximum réduit
nbCible->setMinimum(3);    // Minimum réduit
nbCible->setSingleStep(5); // Pas de 5 au lieu de 1
```

### Résultat
L'utilisateur peut choisir entre 3 et 50 cibles, par pas de 5 (3, 8, 13, 18...).

---

## 6. Agrandir la zone de test (scène graphique)

### Avant
```cpp
scene->setSceneRect(QRect(0, 0, graphicView->width() - 64, 256));
```

### Après
```cpp
scene->setSceneRect(QRect(0, 0, graphicView->width() - 20, graphicView->height() - 20));
```

### Résultat
La scène de test occupe presque toute la taille du widget graphique au lieu d'être limitée à 256px de hauteur. Les cibles peuvent apparaître sur une surface plus grande.

---

## 7. Changer les couleurs globales de l'application

### Avant (fittsview.h)
```cpp
static const QColor color_blue = QColor(0, 218, 199);
static const QColor color_purple = QColor(189, 136, 247);
static const QColor color_red = QColor(212, 102, 121);
```

### Après
```cpp
static const QColor color_blue = QColor(66, 133, 244);    // Bleu Google
static const QColor color_purple = QColor(156, 39, 176);  // Violet Material
static const QColor color_red = QColor(244, 67, 54);      // Rouge Material
```

### Résultat
Les couleurs des cercles (démarrage = bleu, cibles = rouge) et des courbes des graphiques changent dans toute l'application. Ces constantes sont utilisées à la fois dans le contrôleur et dans la vue.

---

## 8. Ajouter un bouton "Recommencer" sur l'écran de résultats

### Avant (resultsScreenWidget - boutons du bas)
```cpp
btnBackFromResults = new QPushButton("Revenir au test");
btnBackFromResults->setIcon(QIcon(":/icons/arrow_back"));
// ...
btnGoToJson = new QPushButton("Voir le JSON");
```

### Après
```cpp
btnBackFromResults = new QPushButton("Revenir au test");
btnBackFromResults->setIcon(QIcon(":/icons/arrow_back"));
btnBackFromResults->setCursor(Qt::PointingHandCursor);
bottomActionsResultLayout->addWidget(btnBackFromResults);

QPushButton *btnRestart = new QPushButton("Recommencer avec les mêmes paramètres");
btnRestart->setIcon(QIcon(":/icons/refresh")); // Ajouter l'icône dans les ressources
btnRestart->setCursor(Qt::PointingHandCursor);
connect(btnRestart, &QPushButton::clicked, fittsController, SLOT(initEvalScreen()));
bottomActionsResultLayout->addWidget(btnRestart);

btnGoToJson = new QPushButton("Voir le JSON");
```

### Résultat
Un troisième bouton apparaît sur l'écran de résultats, permettant de relancer immédiatement le test avec les mêmes paramètres sans repasser par les réglages.

---

## 9. Ajouter des tooltips sur les graphiques

### Avant
```cpp
plotHome = new QChartView;
plotHome->setRenderHint(QPainter::Antialiasing);
plotHome->setRubberBand(QChartView::RectangleRubberBand);
```

### Après
```cpp
plotHome = new QChartView;
plotHome->setRenderHint(QPainter::Antialiasing);
plotHome->setRubberBand(QChartView::RectangleRubberBand);
plotHome->setToolTip("Cliquez et glissez pour zoomer. Clic droit pour réinitialiser.");
```

### Résultat
Un tooltip apparaît au survol du graphique pour indiquer à l'utilisateur qu'il peut zoomer avec le rubber band.

---

## 10. Changer les icônes des boutons de l'écran d'accueil

### Avant
```cpp
buttonStartFitts->setIcon(QIcon(":/img/fitts"));
buttonStartFitts->setIconSize(QSize(128, 128));
// ...
buttonStartKeystroke->setIcon(QIcon(":/img/keystroke"));
buttonStartKeystroke->setIconSize(QSize(128, 128));
```

### Après
```cpp
buttonStartFitts->setIcon(QIcon(":/img/fitts"));
buttonStartFitts->setIconSize(QSize(64, 64)); // Icônes plus petites
// ...
buttonStartKeystroke->setIcon(QIcon(":/img/keystroke"));
buttonStartKeystroke->setIconSize(QSize(64, 64));
```

### Résultat
Les icônes sur l'écran d'accueil passent de 128×128 à 64×64 pixels. L'écran d'accueil paraît plus compact.

---

## 11. Rendre la fenêtre non redimensionnable

### Avant
```cpp
this->resize(640, 480);
```

### Après
```cpp
this->setFixedSize(640, 480);
```

### Résultat
La fenêtre a une taille fixe de 640×480. L'utilisateur ne peut plus la redimensionner. Le bouton maximiser de la barre de titre est grisé.

---

## 12. Ajouter un raccourci clavier pour lancer le test

### Avant
```cpp
startBtn = new QPushButton("Enregistrer les paramètres");
startBtn->setIcon(QIcon(":/icons/save"));
```

### Après
```cpp
startBtn = new QPushButton("Enregistrer les paramètres (Ctrl+Entrée)");
startBtn->setIcon(QIcon(":/icons/save"));
startBtn->setShortcut(QKeySequence("Ctrl+Return"));
```

### Résultat
L'utilisateur peut appuyer sur Ctrl+Entrée pour enregistrer les paramètres et lancer le test, sans avoir à cliquer sur le bouton.
