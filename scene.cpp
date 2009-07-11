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
                itemUnderLine = qgraphicsitem_cast<SceneItem*>(collidingItem);
                if (itemUnderLine->signalType() != SceneItem::Invalid && itemUnderLine->signalType() != SceneItem::Locked) {
                    line = new QGraphicsLineItem(QLineF(collidingItem->pos() + collidingItem->boundingRect().center(),
                                                        collidingItem->pos() + collidingItem->boundingRect().center()), NULL, this);
                    line->setPen(QPen(Qt::blue, 1));
                    line->setZValue(-1.0);

                }
                else if (!bubble) {
                    BubbleItem::BubbleIcon icon = BubbleItem::Invalid;
                    bubble = new BubbleItem(icon);
                    bubble->setZValue(2.0);
                    addItem(bubble);
                    int widthMod = (collidingItem->boundingRect().width() / 2 - bubble->boundingRect().width() / 2);
                    int heightMod = (collidingItem->boundingRect().height() / 2 - bubble->boundingRect().height() / 2);
                    bubble->setPos(collidingItem->x() + widthMod, collidingItem->y() + heightMod);
                }
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
                    case SceneItem::OrGate:
                    case SceneItem::NorGate:
                    case SceneItem::NotGate:
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
                SceneItem *collidingItem = qgraphicsitem_cast<SceneItem*>(collidingItemAt(mouseEvent->scenePos()));
                if (collidingItem && itemUnderLine && (collidingItem != itemUnderLine) &&
                   qgraphicsitem_cast<SceneItem*>(collidingItem)->signalType() != SceneItem::Invalid) {
                    newLine.setPoints(line->line().p1(), collidingItem->pos() + collidingItem->boundingRect().center());

                    SceneItem::SignalType validSignalTypes = validSignalTypesFromTo(itemUnderLine, collidingItem);
                    BubbleItem::BubbleIcon icon;

                    if (validSignalTypes == SceneItem::SenderAndReceiver) {
                        // Only supports left and right; this suffices for our current sender/receiver items (logic gates)
                        int rightFromCenter = mouseEvent->scenePos().x() - collidingItem->x() - collidingItem->boundingRect().width() / 2;
                        icon = rightFromCenter < 0 ? BubbleItem::Input : BubbleItem::Output;
                        if (bubble && bubble->icon() != icon) {
                            bubble->deleteItem();
                            bubble = NULL;
                        }
                    }

                    if (!bubble) {
                        if (itemUnderLine->attachedDevices().contains(collidingItem))
                            icon = BubbleItem::Invalid;
                        else if (validSignalTypes == SceneItem::Invalid || validSignalTypes == SceneItem::Locked)
                            icon = BubbleItem::Invalid;
                        else if (validSignalTypes == SceneItem::Receiver)
                            icon = BubbleItem::Input;
                        else if (validSignalTypes == SceneItem::Sender)
                            icon = BubbleItem::Output;
                        bubble = new BubbleItem(icon);
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

                        int validSides = (bubble->icon() == BubbleItem::Input ? collidingItem->inputSides() : collidingItem->outputSides());
                        int horizontalSide = SceneItem::None, verticalSide = SceneItem::None;

                        if (validSides & SceneItem::Left && validSides & SceneItem::Right) // Horizontal
                            horizontalSide = (right < 0 ? SceneItem::Right : SceneItem::Left);
                        else if (validSides & SceneItem::Left || validSides & SceneItem::Right)
                            horizontalSide = validSides & SceneItem::Left ? SceneItem::Left : SceneItem::Right;
                        if (validSides & SceneItem::Top && validSides & SceneItem::Bottom) // Vertical
                            verticalSide = (bottom < 0 ? SceneItem::Bottom : SceneItem::Top);
                        else if (validSides & SceneItem::Top || validSides & SceneItem::Bottom)
                            verticalSide = validSides & SceneItem::Top ? SceneItem::Top : SceneItem::Bottom;
                        if (verticalSide && horizontalSide)
                            side = (qAbs(right) >= qAbs(bottom) ? horizontalSide : verticalSide);
                        else
                            side = (horizontalSide ? horizontalSide : verticalSide);

                        if (side == SceneItem::Left)
                            bubble->setPos(collidingItem->x() - bubble->boundingRect().width(), collidingItem->y() + heightMod);
                        else if (side == SceneItem::Right)
                            bubble->setPos(collidingItem->x() + collidingItem->boundingRect().width(), collidingItem->y() + heightMod);
                        else if (side == SceneItem::Top)
                            bubble->setPos(collidingItem->x() + widthMod, collidingItem->y() - bubble->boundingRect().height());
                        else if (side == SceneItem::Bottom)
                            bubble->setPos(collidingItem->x() + widthMod, collidingItem->y() + collidingItem->boundingRect().height());
                        else
                            qDebug() << "could not find valid side!";
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

                    // A sender and receiver is required
                    if (((itemUnderLine->signalType() & SceneItem::Sender) && (collidingItem->signalType() & SceneItem::Receiver)) ||
                        ((itemUnderLine->signalType() & SceneItem::Receiver) && (collidingItem->signalType() & SceneItem::Sender))) {

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
            }
            if (bubble) {
                bubble->deleteItem();
                bubble = NULL;
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
