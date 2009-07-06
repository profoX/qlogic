#include <QDebug>

#include <QGraphicsSceneMouseEvent>

#include "scene.h"
#include "sceneitem.h"

Scene::Scene(QObject *parent) : QGraphicsScene(parent) {
    item = NULL;
    line = NULL;
}

void Scene::setMode(Mode mode) {
    myMode = mode;
    switch (mode) {
        case InsertLine:
            if (line)
                delete line;
            line = NULL;
            break;
        default:
            break;
    }
}

void Scene::deleteItem() {
    if (item) {
        item->deleteItem();
        item = NULL;
    }
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (mouseEvent->button() != Qt::LeftButton)
        return;

    switch (myMode) {
        case InsertItem:
            if (!item->checkCollision()) {
                item->setZValue(0.0);
                item->setOpacity(1.0);
                item = NULL;
            }
        case InsertLine:
            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                                mouseEvent->scenePos()), NULL, this);
            line->setPen(QPen(Qt::black, 4));
            line->setZValue(-1.0);
            break;
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
                item = new SceneItem(itemType);
                item->setZValue(1.0);
                addItem(item);
            }
            item->setPos(mouseEvent->scenePos().x() - static_cast<int>(mouseEvent->scenePos().x()) % 50,
                         mouseEvent->scenePos().y() - static_cast<int>(mouseEvent->scenePos().y()) % 50);
            item->checkCollision();
            break;
        case InsertLine:
            if (line) {
                QLineF newLine(line->line().p1(), mouseEvent->scenePos());
                line->setLine(newLine);
            }
            break;
        default:
            break;
    }
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    switch (myMode) {
        case InsertLine:
            delete line;
            line = NULL;
            break;
        default:
            break;
    }
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
