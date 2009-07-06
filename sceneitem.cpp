#include <QDebug>

#include <QSvgRenderer>
#include <QTimeLine>
#include <QGraphicsSceneMouseEvent>
#include <QBrush>

#include "sceneitem.h"
#include "scene.h"

SceneItem::SceneItem(Type type) {
    myType = type;
    on = false;
    overlayItem = new QGraphicsRectItem(this);
    setFlags(QGraphicsItem::ItemDoesntPropagateOpacityToChildren | QGraphicsItem::ItemIsMovable);

    changeSvg();

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
    if (collidingItems().isEmpty() || (collidingItems().count() == 1 && collidingItems()[0] == overlayItem)) {
        hideOverlayItem();
        return false;
    } else {
        showOverlayItem();
        return true;
    }
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
        setPos(mouseEvent->scenePos().x() - static_cast<int>(mouseEvent->scenePos().x()) % 50,
               mouseEvent->scenePos().y() - static_cast<int>(mouseEvent->scenePos().y()) % 50);
        checkCollision();
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
            setPos(oldPosition);
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
