#include <QDebug>

#include <QSvgRenderer>
#include <QTimeLine>
#include <QGraphicsSceneMouseEvent>
#include <QBrush>

#include "sceneitem.h"
#include "scene.h"
#include "line.h"

SceneItem::SceneItem(ItemType type) {
    myType = type;

    switch (myType) {
        case Switch:
            mySignalType = Sender;
            break;
        case Led:
            mySignalType = Receiver;
            break;
        default:
            break;
    }

    on = false;
    overlayItem = new QGraphicsRectItem(this);
    setFlags(QGraphicsItem::ItemDoesntPropagateOpacityToChildren | QGraphicsItem::ItemIsMovable);
    changeSvg();
    setData(0, QVariant("Item"));

    setOpacity(0.0);
    timeLine = new QTimeLine(333, this);
    timeLine->setFrameRange(0, 75);
    connect(timeLine, SIGNAL(frameChanged(int)), this, SLOT(setItemOpacity(int)));
    timeLine->start();
}

void SceneItem::showOverlayItem() {
    overlayItem->setRect(boundingRect());
    overlayItem->setBrush(QBrush(QColor(255, 0, 0, 150)));
    overlayItem->show();
}

void SceneItem::hideOverlayItem() {
    overlayItem->hide();
}

bool SceneItem::checkCollision() {
    if (collidingItems().isEmpty()) {
        hideOverlayItem();
        return false;
    } else {
        QListIterator<QGraphicsItem*> colliding(collidingItems());
        while (colliding.hasNext()) {
            QGraphicsItem *collidingItem = colliding.next();
            if (collidingItem != overlayItem && !attachedWires.contains(qgraphicsitem_cast<Line*>(collidingItem))) {
                showOverlayItem();
                return true;
            }
        }
        hideOverlayItem();
        return false;
    }
}

void SceneItem::attachWire(Line *wire) {
    attachedWires << wire;
}

void SceneItem::deleteItem() {
    timeLine->setFrameRange(static_cast<int>(opacity() * 100.0), 0);
    connect(timeLine, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(timeLine, SIGNAL(frameChanged(int)), this, SLOT(setItemOpacity(int)));
    if (timeLine->state() == QTimeLine::Running)
        timeLine->stop();
    timeLine->start();
}

void SceneItem::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (qobject_cast<Scene*>(scene())->mode() == Scene::MoveItem) {
        setZValue(1.0);
        oldPosition = scenePos();
    }
    QGraphicsItem::mousePressEvent(mouseEvent);
}

void SceneItem::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (qobject_cast<Scene*>(scene())->mode() == Scene::MoveItem) {
        QPointF newPosition(static_cast<int>(mouseEvent->scenePos().x() - static_cast<int>(mouseEvent->scenePos().x()) % 50),
                            static_cast<int>(mouseEvent->scenePos().y() - static_cast<int>(mouseEvent->scenePos().y()) % 50));
        moveWithWires(newPosition);
        checkCollision();
    }
}

void SceneItem::moveWithWires(QPointF newPosition) {
    setPos(newPosition);

    // Move attached wires as well
    QListIterator<Line*> wires(attachedWires);
    while (wires.hasNext()) {
        Line *wire = wires.next();
        wire->setLine(QLineF(wire->sender()->pos() + wire->sender()->boundingRect().center(),
                             wire->receiver()->pos() + wire->receiver()->boundingRect().center()));
    }
}

void SceneItem::changeSvg() {
    QStringList svgs;
    QSvgRenderer *sharedRenderer;

    renderer()->deleteLater();
    switch (myType) {
        case Switch:
            svgs << ":/res/img/switch_on.svg" << ":/res/img/switch.svg";
            break;
        case Led:
            svgs << ":/res/img/led_on.svg" << ":/res/img/led.svg";
            break;
        default:
            break;
    }

    if (on)
        sharedRenderer = new QSvgRenderer(svgs[0], this);
    else
        sharedRenderer = new QSvgRenderer(svgs[1], this);
    setSharedRenderer(sharedRenderer);
}

void SceneItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (qobject_cast<Scene*>(scene())->mode() == Scene::MoveItem) {
        if (checkCollision()) {
            moveWithWires(oldPosition);
            hideOverlayItem();
        } else if (myType == Switch && oldPosition == scenePos()) {
            on = !on;
            changeSvg();
        }
        setZValue(0.0);
    }
    QGraphicsItem::mouseReleaseEvent(mouseEvent);
}

void SceneItem::setItemOpacity(int currentOpacity) {
    setOpacity(currentOpacity/100.0);
}

void SceneItem::lockOpacity(qreal opacity) {
    timeLine->stop();
    setOpacity(opacity);
}
