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
                    line = new QGraphicsLineItem(QLineF(collidingItem->pos() + collidingItem->boundingRect().center(),
                                                        collidingItem->pos() + collidingItem->boundingRect().center()), NULL, this);
                    line->setPen(QPen(Qt::black, 1));
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
                    BubbleItem::BubbleIcon icon, icon2;

                    if (validSignalTypes == SceneItem::SenderAndReceiver) {
                        if (itemUnderLine->signalType() == SceneItem::Sender) {
                            icon = BubbleItem::Input;
                            icon2 = BubbleItem::Output;
                        } else if (itemUnderLine->signalType() == SceneItem::Receiver) {
                            icon = BubbleItem::Output;
                            icon2 = BubbleItem::Input;
                        }
                        else {
                            // Only supports left and right; this suffices for our current sender/receiver items (logic gates)
                            int rightFromCenter = mouseEvent->scenePos().x() - collidingItem->x() - collidingItem->boundingRect().width() / 2;
                            icon = rightFromCenter < 0 ? BubbleItem::Input : BubbleItem::Output;
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

                    QList<SceneItem::Sides> wire = shortestPossibleWire(itemUnderLine, collidingItem);
                    if (wire.isEmpty()) {
                        bubble->setPos(collidingItem->x() + widthMod, collidingItem->y() + heightMod);
                        bubble2->hide();
                    } else {
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
                bubble2->deleteItem();
                bubble2 = NULL;
            }
            break;
        default:
            break;
    }
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

QList<SceneItem::Sides> Scene::shortestPossibleWire(SceneItem *source, SceneItem *target) {
    QList<SceneItem::Sides> wire;

    // A valid wire can only be drawn when one item can act as sender and the other as receiver
    SceneItem::SignalType validSignalTypes = validSignalTypesFromTo(source, target);
    if (source->attachedDevices().contains(target) || validSignalTypes == SceneItem::Invalid ||
        validSignalTypes == SceneItem::Locked)
        return wire;

    // Valid sides for items
    int validSidesTarget = (bubble->icon() == BubbleItem::Input ? target->inputSides() : target->outputSides());
    int validSidesSource = (bubble->icon() == BubbleItem::Input ? source->outputSides() : source->inputSides());

    // Set up connection preference based on item locations
    QList<SceneItem::Sides> preferredTargetSides, preferredSourceSides;
    SceneItem::Sides leastPreferredTargetSide, leastPreferredSourceSide;
    int right = (target->x() + target->boundingRect().width() / 2) - (source->x() + source->boundingRect().width() / 2);
    int bottom = (target->y() + target->boundingRect().height() / 2) - (source->y() + source->boundingRect().height() / 2);

    if (qAbs(bottom) > qAbs(right)) { // Vertical preference
        if (bottom >= 0) { // Target item is below source item
            leastPreferredTargetSide = SceneItem::Bottom;
            preferredTargetSides.prepend(SceneItem::Top);
            leastPreferredSourceSide = SceneItem::Top;
            preferredSourceSides.prepend(SceneItem::Bottom);
        } else {
            leastPreferredTargetSide = SceneItem::Top;
            preferredTargetSides.prepend(SceneItem::Bottom);
            leastPreferredSourceSide = SceneItem::Bottom;
            preferredSourceSides.prepend(SceneItem::Top);
        }
        if (right >= 0) { // Target item is to the right from source item
            preferredTargetSides.append(SceneItem::Left);
            preferredTargetSides.append(SceneItem::Right);
            preferredSourceSides.append(SceneItem::Right);
            preferredSourceSides.append(SceneItem::Left);
        } else {
            preferredTargetSides.append(SceneItem::Right);
            preferredTargetSides.append(SceneItem::Left);
            preferredSourceSides.append(SceneItem::Left);
            preferredSourceSides.append(SceneItem::Right);
        }
    } else { // Horizontal preference
        if (right >= 0) { // Target item is to the right from source item
            leastPreferredTargetSide = SceneItem::Right;
            preferredTargetSides.prepend(SceneItem::Left);
            leastPreferredSourceSide = SceneItem::Left;
            preferredSourceSides.prepend(SceneItem::Right);
        } else {
            leastPreferredTargetSide = SceneItem::Left;
            preferredTargetSides.prepend(SceneItem::Right);
            leastPreferredSourceSide = SceneItem::Right;
            preferredSourceSides.prepend(SceneItem::Left);
        }
        if (bottom >= 0) { // Target item is below source item
            preferredTargetSides.append(SceneItem::Top);
            preferredTargetSides.append(SceneItem::Bottom);
            preferredSourceSides.append(SceneItem::Bottom);
            preferredSourceSides.append(SceneItem::Top);
        } else {
            preferredTargetSides.append(SceneItem::Bottom);
            preferredTargetSides.append(SceneItem::Top);
            preferredSourceSides.append(SceneItem::Top);
            preferredSourceSides.append(SceneItem::Bottom);
        }
    }
    preferredTargetSides.append(leastPreferredTargetSide);
    preferredSourceSides.append(leastPreferredSourceSide);

    // Calculate valid side of source and target item closest to eachother
    bool found = false;
    for (int i = 0; !found && i < preferredSourceSides.count(); ++i) {
        if (validSidesSource & preferredSourceSides[i]) {
            wire.append(preferredSourceSides[i]);
            found = true;
        }
    }
    found = false;
    for (int i = 0; !found && i < preferredTargetSides.count(); ++i) {
        if (validSidesTarget & preferredTargetSides[i]) {
            wire.append(preferredTargetSides[i]);
            found = true;
        }
    }

    if (wire.count() != 2)
        wire.clear();
    return wire;
}

SceneItem::SignalType Scene::validSignalTypesFromTo(SceneItem *from, SceneItem *to) {
    if ((from->signalType() == to->signalType() && to->signalType() != SceneItem::SenderAndReceiver))
        return SceneItem::Invalid;
    else
        return to->signalType();
}
