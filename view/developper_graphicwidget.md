# Documentation Développeur - GraphicWidget

## Localisation
`/fitts/view/graphicwidget.h` et `/fitts/view/graphicwidget.cpp`

## Rôle du fichier
Widget personnalisé qui étend `QGraphicsView` pour **capturer les clics souris** et les transmettre au contrôleur via un signal Qt. C'est le composant qui permet à l'utilisateur de cliquer sur les cibles pendant le test de Fitts.

## Héritage
```
QGraphicsView
    └── GraphicWidget
```

`QGraphicsView` est le widget Qt standard pour afficher une `QGraphicsScene`. `GraphicWidget` y ajoute simplement la détection des clics.

## Signal

### `mouseClicked(int x, int y)`
Émis à chaque pression de souris sur le widget. Les coordonnées `x` et `y` sont celles de l'événement (en pixels, relatives au widget).

Ce signal est connecté au slot `FittsController::cibleClicked(int, int)` dans le constructeur de `FittsView`.

## Fonctions

### `GraphicWidget()`
Constructeur vide. Aucune initialisation spécifique.

### `mousePressEvent(QMouseEvent *event)` (protected)
Surcharge de `QGraphicsView::mousePressEvent`. Intercepte le clic et émet le signal :

```cpp
void GraphicWidget::mousePressEvent(QMouseEvent *event) {
    emit mouseClicked(event->x(), event->y());
}
```

**Important :** La méthode parente `QGraphicsView::mousePressEvent(event)` n'est **pas** appelée. Cela signifie que les comportements par défaut de `QGraphicsView` (sélection d'items, drag, etc.) sont désactivés.

## Flux de données

```
Clic souris
    → GraphicWidget::mousePressEvent()
        → signal mouseClicked(x, y)
            → FittsController::cibleClicked(x, y)
                → mapToScene(x, y)  // Conversion coordonnées widget → scène
                → Vérification distance au cercle
```

## Ce qu'on peut modifier

### Détecter aussi le bouton droit
```cpp
void GraphicWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton)
        emit mouseClicked(event->x(), event->y());
}
```

### Ajouter un signal de mouvement souris
Dans le `.h` :
```cpp
signals:
    void mouseClicked(int, int);
    void mouseMoved(int, int);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
```
Dans le `.cpp` :
```cpp
void GraphicWidget::mouseMoveEvent(QMouseEvent *event) {
    emit mouseMoved(event->x(), event->y());
}
```

### Ajouter un retour visuel au survol
```cpp
void GraphicWidget::mouseMoveEvent(QMouseEvent *event) {
    setCursor(Qt::CrossCursor);
}
```

### Restaurer le comportement par défaut de QGraphicsView
Ajouter l'appel à la méthode parente :
```cpp
void GraphicWidget::mousePressEvent(QMouseEvent *event) {
    emit mouseClicked(event->x(), event->y());
    QGraphicsView::mousePressEvent(event); // Propager l'événement
}
```

## Dépendances
- `<QGraphicsView>` : Classe parente
- `<QMouseEvent>` : Événements souris

## Points d'attention
- **`event->x()` et `event->y()` sont dépréciés** depuis Qt 5.15. Utiliser `event->position().x()` et `event->position().y()` pour Qt 6.
- **Pas de propagation** : L'événement souris n'est pas transmis à `QGraphicsView`, ce qui désactive le drag & zoom natif. C'est voulu ici car seul le clic nous intéresse.
- **Pas de distinction gauche/droite** : Tout type de clic (gauche, droit, molette) émet le signal.
