#include "view.h"

View::View(QWidget *parent) : QGraphicsView(parent) {
}

void View::leaveEvent(QEvent *event) {
    Q_UNUSED(event);
    emit mouseLeftView();
}

void View::zoom(int zoomLevel) {
    resetTransform();
    scale(0.5, 0.5);
    scale(zoomLevel/10.0, zoomLevel/10.0);
}
