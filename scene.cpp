#include <QDebug>

#include <QGraphicsSceneMouseEvent>

#include "scene.h"
#include "sceneitem.h"
#include "line.h"

Scene::Scene(QObject *parent) : QGraphicsScene(parent) {
    item = NULL;
    line = NULL;
    itemUnderLine = NULL;
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

QGraphicsItem *Scene::collidingItemAt(QPointF pos) {
    if (itemAt(pos) == 0)
        return NULL;
    else if (itemAt(pos)->data(0).toString() == "Item")
        return itemAt(pos);
    return NULL;
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (mouseEvent->button() != Qt::LeftButton)
        return;

    QGraphicsItem *collidingItem;
    switch (myMode) {
        case InsertItem:
            if (!item->checkCollision()) {
                item->setZValue(0.0);
                item->setOpacity(1.0);
                item = NULL;
            }
        case InsertLine:
            collidingItem = collidingItemAt(mouseEvent->scenePos());
            if (collidingItem) {
                line = new QGraphicsLineItem(QLineF(collidingItem->pos() + collidingItem->boundingRect().center(),
                                                    collidingItem->pos() + collidingItem->boundingRect().center()), NULL, this);
                line->setPen(QPen(Qt::blue, 1));
                line->setZValue(-1.0);
                itemUnderLine = qgraphicsitem_cast<SceneItem*>(collidingItem);
            }
            break;
        default:
            break;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    QList<QGraphicsItem *> collidingItems;
    QPointF newPosition;
    switch (myMode) {
        case InsertItem:
            if (!item) {
                item = new SceneItem(itemType);
                item->setZValue(1.0);
                addItem(item);
            }
            newPosition.setX(static_cast<int>(mouseEvent->scenePos().x() - static_cast<int>(mouseEvent->scenePos().x()) % 50));
            newPosition.setY(static_cast<int>(mouseEvent->scenePos().y() - static_cast<int>(mouseEvent->scenePos().y()) % 50));
            item->setPos(newPosition);
            item->checkCollision();
            break;
        case InsertLine:
            if (line) {
                QLineF newLine;
                QGraphicsItem *collidingItem = collidingItemAt(mouseEvent->scenePos());
                if (collidingItem)
                    newLine.setPoints(line->line().p1(), collidingItem->pos() + collidingItem->boundingRect().center());
                else
                    newLine.setPoints(line->line().p1(), mouseEvent->scenePos());
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
            if (line) {
                SceneItem *collidingItem = qgraphicsitem_cast<SceneItem*>(collidingItemAt(mouseEvent->scenePos()));
                if (collidingItem) {
                    // Both can't be sender and both can't be receiver
                    if (!((itemUnderLine->signalType() == SceneItem::Sender && collidingItem->signalType() == SceneItem::Sender) ||
                        (itemUnderLine->signalType() == SceneItem::Receiver && collidingItem->signalType() == SceneItem::Receiver))) {
                        Line *wire = new Line(itemUnderLine, collidingItem);
                        wire->setLine(line->line());
                        addItem(wire);
                    }
                }
                delete line;
                line = NULL;
                itemUnderLine = NULL;
            }
            break;
        default:
            break;
    }
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
