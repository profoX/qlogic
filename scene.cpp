#include <QDebug>

#include <QGraphicsSceneMouseEvent>

#include "scene.h"
#include "sceneitem.h"

Scene::Scene(QObject *parent) : QGraphicsScene(parent) {
    item = NULL;
    overlayItem = new QGraphicsRectItem(NULL, this);
}

void Scene::deleteItem() {
    if (item) {
        overlayItem->setParentItem(NULL);
        overlayItem->hide();
        item->deleteItem();
        item = NULL;
    }
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    switch (mode) {
        case InsertItem:
            if (!overlayItem->isVisible()) {
                overlayItem->setParentItem(NULL);
                overlayItem->hide();
                item->setZValue(0.0);
                item->setOpacity(1.0);
                item = NULL;
            }
        default:
            break;
    }
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    QList<QGraphicsItem *> collidingItems;
    switch (mode) {
        case InsertItem:
            if (!item) {
                item = new SceneItem(SceneItem::Switch);
                item->setZValue(1.0);
                addItem(item);
            }
            item->setPos(mouseEvent->scenePos().x() - static_cast<int>(mouseEvent->scenePos().x()) % 50,
                         mouseEvent->scenePos().y() - static_cast<int>(mouseEvent->scenePos().y()) % 50);
            collidingItems = item->collidingItems();
            if (collidingItems.isEmpty() ||
                (collidingItems.count() == 1 && collidingItems[0] == overlayItem)) {
                overlayItem->hide();
            } else {
                overlayItem->setRect(item->boundingRect());
                overlayItem->setBrush(QBrush(QColor(255, 0, 0, 150)));
                overlayItem->setParentItem(item);
                overlayItem->show();
            }
            break;
        default:
            break;
    }
}
