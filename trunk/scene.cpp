#include <QDebug>

#include <QGraphicsSceneMouseEvent>

#include "scene.h"
#include "sceneitem.h"

Scene::Scene(QObject *parent) : QGraphicsScene(parent) {
    item = NULL;
}

void Scene::deleteItem() {
    if (item) {
        item->deleteItem();
        item = NULL;
    }
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    switch (myMode) {
        case InsertItem:
            if (!item->checkCollision()) {
                item->setZValue(0.0);
                item->setOpacity(1.0);
                item = NULL;
            }
        default:
            break;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    QList<QGraphicsItem *> collidingItems;
    switch (myMode) {
        case InsertItem:
            if (!item) {
                item = new SceneItem(SceneItem::Switch);
                item->setZValue(1.0);
                addItem(item);
            }
            item->setPos(mouseEvent->scenePos().x() - static_cast<int>(mouseEvent->scenePos().x()) % 50,
                         mouseEvent->scenePos().y() - static_cast<int>(mouseEvent->scenePos().y()) % 50);
            item->checkCollision();
            break;
        default:
            break;
    }
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}
