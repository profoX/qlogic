#include <QDebug>

#include <QGraphicsSceneMouseEvent>
#include <QMenu>

#include "scene.h"
#include "line.h"

#include "inputdevice.h"
#include "outputdevice.h"
#include "logicgate.h"
#include "bubbleitem.h"

Scene::Scene(QMenu *menu, QObject *parent) : QGraphicsScene(parent) {
    item = NULL;
    line = NULL;
    itemUnderLine = NULL;
    itemMenu = menu;
    bubble = NULL;
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
    else {
        QListIterator<QGraphicsItem*> colliding(items(pos));
        QGraphicsItem *collidingItem;
        while (colliding.hasNext()) {
            collidingItem = colliding.next();
            if (collidingItem->data(0).toString() == "Item")
                return collidingItem;
        }
    }
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
                item->lockOpacity(1.0);
                item->initAfterCreation();
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
                switch (itemType) {
                    case SceneItem::Switch:
                    case SceneItem::Oscillator:
                        item = new InputDevice(itemType, itemMenu);
                        break;
                    case SceneItem::Led:
                        item = new OutputDevice(itemType, itemMenu);
                        break;
                    case SceneItem::AndGate:
                    case SceneItem::NandGate:
                        item = new LogicGate(itemType, itemMenu);
                        break;
                    default:
                        break;
                }
                item->setZValue(1.0);
                addItem(item);
            }
            // Move per 50 pixels
            newPosition.setX(static_cast<int>(mouseEvent->scenePos().x() - static_cast<int>(mouseEvent->scenePos().x()) % 50));
            newPosition.setY(static_cast<int>(mouseEvent->scenePos().y() - static_cast<int>(mouseEvent->scenePos().y()) % 50));
            item->setPos(newPosition);
            item->checkCollision();
            break;
        case InsertLine:
            if (line) {
                QLineF newLine;
                QGraphicsItem *collidingItem = collidingItemAt(mouseEvent->scenePos());
                if (collidingItem && itemUnderLine && (collidingItem != itemUnderLine) &&
                    qgraphicsitem_cast<SceneItem*>(collidingItem)->signalType() != SceneItem::Invalid) {
                    newLine.setPoints(line->line().p1(), collidingItem->pos() + collidingItem->boundingRect().center());

                    if (!bubble) {
                        SceneItem::SignalType validSignalTypes = validSignalTypesFromTo(itemUnderLine, qgraphicsitem_cast<SceneItem*>(collidingItem));
                        bubble = new BubbleItem(validSignalTypes == SceneItem::Invalid ? BubbleItem::Invalid : BubbleItem::Input);
                        bubble->setZValue(2.0);
                        addItem(bubble);
                    }

                    int widthMod = (collidingItem->boundingRect().width() / 2 - bubble->boundingRect().width() / 2);
                    int heightMod = (collidingItem->boundingRect().height() / 2 - bubble->boundingRect().height() / 2);
                    if (bubble->icon() == BubbleItem::Invalid) {
                        bubble->setPos(collidingItem->x() + widthMod, collidingItem->y() + heightMod);
                    } else {
                        // Calculate shortest connection
                        int side = SceneItem::None;
                        int right = collidingItem->x() - (itemUnderLine->x() + itemUnderLine->boundingRect().width() / 2);
                        int bottom = collidingItem->y() - (itemUnderLine->y() + itemUnderLine->boundingRect().height() / 2);

                        if (qAbs(right) >= qAbs(bottom))
                            side = (right < 0 ? SceneItem::Left : SceneItem::Right);
                        else
                            side = (bottom < 0 ? SceneItem::Top : SceneItem::Bottom);
                        if (side == SceneItem::Right)
                            bubble->setPos(collidingItem->x() - bubble->boundingRect().width(), collidingItem->y() + heightMod);
                        else if (side == SceneItem::Left)
                            bubble->setPos(collidingItem->x() + collidingItem->boundingRect().width(), collidingItem->y() + heightMod);
                        else if (side == SceneItem::Bottom)
                            bubble->setPos(collidingItem->x() + widthMod, collidingItem->y() - bubble->boundingRect().height());
                        else if (side == SceneItem::Top)
                            bubble->setPos(collidingItem->x() + widthMod, collidingItem->y() + collidingItem->boundingRect().height());
                    }
                } else {
                    if (bubble) {
                        bubble->deleteItem();
                        bubble = NULL;
                    }
                    newLine.setPoints(line->line().p1(), mouseEvent->scenePos());
                }
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
                if (collidingItem && itemUnderLine && (collidingItem != itemUnderLine) &&
                    qgraphicsitem_cast<SceneItem*>(collidingItem)->signalType() != SceneItem::Invalid) {
                    // Both can't be sender and both can't be receiver
                    if (!((itemUnderLine->signalType() == SceneItem::Sender && collidingItem->signalType() == SceneItem::Sender) ||
                        (itemUnderLine->signalType() == SceneItem::Receiver && collidingItem->signalType() == SceneItem::Receiver))) {
                        // There's no point in connecting devices which are already connected
                        if (!itemUnderLine->attachedDevices().contains(collidingItem)) {
                            Line *wire = new Line(itemUnderLine, collidingItem);
                            wire->setLine(line->line());
                            addItem(wire);
                        } else
                            qDebug() << "User tried to double-wire";
                    }
                }
                delete line;
                line = NULL;
                itemUnderLine = NULL;
                if (bubble) {
                    bubble->deleteItem();
                    bubble = NULL;
                }
            }
            break;
        default:
            break;
    }
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

SceneItem::SignalType Scene::validSignalTypesFromTo(SceneItem *from, SceneItem *to) {
    if ((from->signalType() == to->signalType() && to->signalType() != SceneItem::SenderAndReceiver))
        return SceneItem::Invalid;
    else
        return to->signalType();
}
