# Modifications possibles - GraphicWidget

## Localisation
`/fitts/view/graphicwidget.h` et `/fitts/view/graphicwidget.cpp`

---

## 1. Filtrer uniquement le clic gauche

### Avant
```cpp
void GraphicWidget::mousePressEvent(QMouseEvent *event) {
    emit mouseClicked(event->x(), event->y());
}
```

### Après
```cpp
void GraphicWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit mouseClicked(event->x(), event->y());
    }
}
```

### Résultat
Seul le clic gauche est pris en compte pendant le test. Les clics droits et molette sont ignorés. Évite les clics accidentels qui fausseraient les mesures.

---

## 2. Compatibilité Qt 6 (corriger la dépréciation)

### Avant
```cpp
void GraphicWidget::mousePressEvent(QMouseEvent *event) {
    emit mouseClicked(event->x(), event->y());
}
```

### Après
```cpp
void GraphicWidget::mousePressEvent(QMouseEvent *event) {
    emit mouseClicked(static_cast<int>(event->position().x()),
                      static_cast<int>(event->position().y()));
}
```

### Résultat
Le code compile sans avertissement de dépréciation sous Qt 6. `event->x()` et `event->y()` sont dépréciés depuis Qt 5.15 au profit de `event->position()`.

---

## 3. Ajouter un signal de relâchement du clic

### Avant (graphicwidget.h)
```cpp
signals:
    void mouseClicked(int, int);
protected:
    void mousePressEvent(QMouseEvent *event);
```

### Après
```cpp
signals:
    void mouseClicked(int, int);
    void mouseReleased(int, int);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
```

Dans `graphicwidget.cpp` :
```cpp
void GraphicWidget::mouseReleaseEvent(QMouseEvent *event) {
    emit mouseReleased(event->x(), event->y());
}
```

### Résultat
On peut maintenant mesurer le temps entre l'appui et le relâchement du bouton (temps de maintien). Utile pour des expériences sur le temps de réaction ou le contrôle moteur fin.

---

## 4. Ajouter le suivi du mouvement de souris

### Avant (graphicwidget.h)
```cpp
class GraphicWidget : public QGraphicsView {
    Q_OBJECT
public:
    GraphicWidget();
signals:
    void mouseClicked(int, int);
protected:
    void mousePressEvent(QMouseEvent *event);
};
```

### Après
```cpp
class GraphicWidget : public QGraphicsView {
    Q_OBJECT
public:
    GraphicWidget();
signals:
    void mouseClicked(int, int);
    void mouseMoved(int, int);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};
```

Dans `graphicwidget.cpp` :
```cpp
GraphicWidget::GraphicWidget() {
    setMouseTracking(true); // Activer le tracking sans bouton enfoncé
}

void GraphicWidget::mouseMoveEvent(QMouseEvent *event) {
    emit mouseMoved(event->x(), event->y());
}
```

### Résultat
On peut tracer la trajectoire de la souris pendant le test. Utile pour analyser le chemin pris par l'utilisateur pour atteindre chaque cible (loi de Fitts étendue). Il faut ensuite connecter le signal dans la vue et stocker les positions dans le modèle.

---

## 5. Ajouter un feedback visuel au survol de la cible

### Avant
```cpp
GraphicWidget::GraphicWidget() {}
```

### Après
```cpp
GraphicWidget::GraphicWidget() {
    setMouseTracking(true);
}

void GraphicWidget::mouseMoveEvent(QMouseEvent *event) {
    QPointF scenePos = mapToScene(event->pos());
    QGraphicsItem *item = scene()->itemAt(scenePos, transform());
    if (item) {
        setCursor(Qt::PointingHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
}
```

### Résultat
Le curseur change en main pointeuse quand il survole un élément (cercle cible) et revient à la flèche normale en dehors. Note : les cibles définissent déjà `setCursor(Qt::PointingHandCursor)` dans le contrôleur, mais ce code ajoute un fallback côté widget.

---

## 6. Propager l'événement à QGraphicsView

### Avant
```cpp
void GraphicWidget::mousePressEvent(QMouseEvent *event) {
    emit mouseClicked(event->x(), event->y());
}
```

### Après
```cpp
void GraphicWidget::mousePressEvent(QMouseEvent *event) {
    emit mouseClicked(event->x(), event->y());
    QGraphicsView::mousePressEvent(event); // Propager à la classe parente
}
```

### Résultat
Les fonctionnalités natives de `QGraphicsView` sont restaurées (sélection d'items, drag & drop, etc.). Actuellement, seul le signal personnalisé est émis. Ajouter cette ligne permet d'utiliser des fonctionnalités avancées de la scène tout en gardant le signal de clic.

---

## 7. Ajouter un compteur de clics par seconde (CPS)

### Avant (graphicwidget.h)
```cpp
class GraphicWidget : public QGraphicsView {
    Q_OBJECT
public:
    GraphicWidget();
signals:
    void mouseClicked(int, int);
```

### Après
```cpp
#include <QElapsedTimer>
#include <QList>

class GraphicWidget : public QGraphicsView {
    Q_OBJECT
public:
    GraphicWidget();
    double getClicksPerSecond() const;
signals:
    void mouseClicked(int, int);
private:
    QList<qint64> clickTimestamps;
    QElapsedTimer cpsTimer;
```

Dans `graphicwidget.cpp` :
```cpp
GraphicWidget::GraphicWidget() {
    cpsTimer.start();
}

void GraphicWidget::mousePressEvent(QMouseEvent *event) {
    clickTimestamps.append(cpsTimer.elapsed());

    // Garder seulement les clics de la dernière seconde
    qint64 now = cpsTimer.elapsed();
    while (!clickTimestamps.isEmpty() && (now - clickTimestamps.first()) > 1000) {
        clickTimestamps.removeFirst();
    }

    emit mouseClicked(event->x(), event->y());
}

double GraphicWidget::getClicksPerSecond() const {
    return clickTimestamps.size();
}
```

### Résultat
Le widget mesure le nombre de clics par seconde en temps réel. On peut afficher cette info dans l'interface pour donner un feedback à l'utilisateur sur sa vitesse de clic.
