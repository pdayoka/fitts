#include "graphicwidget.h"

GraphicWidget::GraphicWidget() {}

void GraphicWidget::mousePressEvent(QMouseEvent *event) {
	emit mouseClicked(event->x(), event->y());
}
