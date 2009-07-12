#include <QDebug>

#include <QGraphicsSceneMouseEvent>
#include <QMenu>

#include "scene.h"
#include "line.h"

#include "inputdevice.h"
#include "outputdevice.h"
#include "logicgate.h"

Scene::Scene(QMenu *menu, QObject *parent) : QGraphicsScene(parent) {
    item = NULL;
    line = NULL;
    itemUnderLine = NULL;
    itemMenu = menu;
    bubble = NULL;
    bubble2 = NULL;
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
                    line = new Line(QLineF(collidingItem->pos() + collidingItem->boundingRect().center(),
                                           collidingItem->pos() + collidingItem->boundingRect().center()), NULL, this);
                    line->setPen(QPen(Qt::black, 1));
                    //line->setZValue(-1.0);
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
            // Move per 25 pixels
            newPosition.setX(static_cast<int>(mouseEvent->scenePos().x() - static_cast<int>(mouseEvent->scenePos().x()) % 25));
            newPosition.setY(static_cast<int>(mouseEvent->scenePos().y() - static_cast<int>(mouseEvent->scenePos().y()) % 25));
            item->setPos(newPosition);
            item->checkCollision();
            break;
        case InsertLine:
            if (line) {
                QLineF newLine;
                SceneItem *collidingItem = qgraphicsitem_cast<SceneItem*>(collidingItemAt(mouseEvent->scenePos()));
                if (collidingItem && itemUnderLine && collidingItem != itemUnderLine && collidingItem->signalType() != SceneItem::Invalid) {
                    newLine.setPoints(line->line().p1(), collidingItem->pos() + collidingItem->boundingRect().center());

                    SceneItem::SignalType validSignalTypes = Line::validSignalTypesFromTo(itemUnderLine, collidingItem);
                    BubbleItem::BubbleIcon icon, icon2;

                    if (validSignalTypes == SceneItem::SenderAndReceiver) {
                        if (itemUnderLine->signalType() == SceneItem::Sender) {
                            icon = BubbleItem::Input;
                            icon2 = BubbleItem::Output;
                        } else if (itemUnderLine->signalType() == SceneItem::Receiver) {
                            icon = BubbleItem::Output;
                            icon2 = BubbleItem::Input;
                        } else { // SceneItem::SenderAndReceiver
                            // Only supports left and right; this suffices for our current sender/receiver items (logic gates)
                            int rightFromCenter = mouseEvent->scenePos().x() - collidingItem->x() - collidingItem->boundingRect().width() / 2;
                            icon = rightFromCenter < 0 ? BubbleItem::Input : BubbleItem::Output;
                            icon2 = rightFromCenter < 0 ? BubbleItem::Output : BubbleItem::Input;
                        }
                        if (bubble && bubble->icon() != icon) {
                            bubble->deleteItem();
                            bubble2->deleteItem();
                            bubble = NULL;
                            bubble2 = NULL;
                        }
                    }

                    if (!bubble) {
                        if (itemUnderLine->attachedDevices().contains(collidingItem) || validSignalTypes == SceneItem::Invalid ||
                            validSignalTypes == SceneItem::Locked) {
                            icon = BubbleItem::Invalid;
                            icon2 = BubbleItem::Invalid;
                        } else if (validSignalTypes == SceneItem::Receiver) {
                            icon = BubbleItem::Input;
                            icon2 = BubbleItem::Output;
                        } else if (validSignalTypes == SceneItem::Sender) {
                            icon = BubbleItem::Output;
                            icon2 = BubbleItem::Input;
                        }
                        bubble = new BubbleItem(icon);
                        bubble->setZValue(2.0);
                        addItem(bubble);
                        bubble2 = new BubbleItem(icon2);
                        bubble2->setZValue(2.0);
                        addItem(bubble2);
                    }

                    int widthMod = (collidingItem->boundingRect().width() / 2 - bubble->boundingRect().width() / 2);
                    int heightMod = (collidingItem->boundingRect().height() / 2 - bubble->boundingRect().height() / 2);

                    QList<SceneItem::Sides> wire = Line::shortestPossibleWire(itemUnderLine, collidingItem, bubble->icon());
                    if (wire.isEmpty()) {
                        bubble->setPos(collidingItem->x() + widthMod, collidingItem->y() + heightMod);
                        bubble2->hide();
                    } else {
                        line->setSides(wire[0], wire[1]);
                        line->setItems(itemUnderLine, collidingItem);
                        bubble2->show();
                        if (wire[0] == SceneItem::Left)
                            bubble2->setPos(itemUnderLine->x() - bubble2->boundingRect().width(), itemUnderLine->y() + heightMod);
                        else if (wire[0] == SceneItem::Right)
                            bubble2->setPos(itemUnderLine->x() + itemUnderLine->boundingRect().width(), itemUnderLine->y() + heightMod);
                        else if (wire[0] == SceneItem::Top)
                            bubble2->setPos(itemUnderLine->x() + widthMod, itemUnderLine->y() - bubble2->boundingRect().height());
                        else if (wire[0] == SceneItem::Bottom)
                            bubble2->setPos(itemUnderLine->x() + widthMod, itemUnderLine->y() + itemUnderLine->boundingRect().height());
                        if (wire[1] == SceneItem::Left)
                            bubble->setPos(collidingItem->x() - bubble->boundingRect().width(), collidingItem->y() + heightMod);
                        else if (wire[1] == SceneItem::Right)
                            bubble->setPos(collidingItem->x() + collidingItem->boundingRect().width(), collidingItem->y() + heightMod);
                        else if (wire[1] == SceneItem::Top)
                            bubble->setPos(collidingItem->x() + widthMod, collidingItem->y() - bubble->boundingRect().height());
                        else if (wire[1] == SceneItem::Bottom)
                            bubble->setPos(collidingItem->x() + widthMod, collidingItem->y() + collidingItem->boundingRect().height());
                    }
                } else {
                    if (bubble) {
                        bubble->deleteItem();
                        bubble = NULL;
                        bubble2->deleteItem();
                        bubble2 = NULL;
                    }
                    line->setSides(SceneItem::None, SceneItem::None);
                    line->setItems(NULL, NULL);
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
                bool deleteLine = true;
                SceneItem *collidingItem = qgraphicsitem_cast<SceneItem*>(collidingItemAt(mouseEvent->scenePos()));
                if (collidingItem && itemUnderLine && (collidingItem != itemUnderLine) &&
                    qgraphicsitem_cast<SceneItem*>(collidingItem)->signalType() != SceneItem::Invalid) {

                    // A sender and receiver is required
                    if (((itemUnderLine->signalType() & SceneItem::Sender) && (collidingItem->signalType() & SceneItem::Receiver)) ||
                        ((itemUnderLine->signalType() & SceneItem::Receiver) && (collidingItem->signalType() & SceneItem::Sender))) {

                        // There's no point in connecting devices which are already connected
                        if (!itemUnderLine->attachedDevices().contains(collidingItem)) {
                            line->setItems(itemUnderLine, collidingItem);
                            line->activate();
                            deleteLine = false;
                            addItem(line);
                        } else
                            qDebug() << "User tried to double-wire";

                    }

                }
                if (deleteLine)
                    delete line;
                line = NULL;
                itemUnderLine = NULL;
            }
            if (bubble) {
                bubble->deleteItem();
                bubble = NULL;
                bubble2->deleteItem();
                bubble2 = NULL;
            }
            break;
        default:
            break;
    }
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
