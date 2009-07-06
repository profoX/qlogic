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

    switch (type) {
        case Switch:
            QSvgRenderer *sharedRenderer = new QSvgRenderer(QString(":/res/img/switch.svg"), this);
            setSharedRenderer(sharedRenderer);
            break;
    }

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
    setZValue(1.0);
    oldPosition = scenePos();
    QGraphicsItem::mousePressEvent(mouseEvent);
}

void SceneItem::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    setPos(mouseEvent->scenePos().x() - static_cast<int>(mouseEvent->scenePos().x()) % 50,
           mouseEvent->scenePos().y() - static_cast<int>(mouseEvent->scenePos().y()) % 50);
    checkCollision();
}

void SceneItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    QSvgRenderer *sharedRenderer;

    if (checkCollision()) {
        setPos(oldPosition);
        hideOverlayItem();
    } else if (oldPosition == scenePos() && qobject_cast<Scene*>(scene())->mode() == Scene::MoveItem) {
        switch (myType) {
            case Switch:
                renderer()->deleteLater();
                if (on)
                    sharedRenderer = new QSvgRenderer(QString(":/res/img/switch.svg"), this);
                else
                    sharedRenderer = new QSvgRenderer(QString(":/res/img/switch_on.svg"), this);
                on = !on;
                setSharedRenderer(sharedRenderer);
                break;
            default:
                break;
        }
    }
    setZValue(0.0);
    QGraphicsItem::mouseReleaseEvent(mouseEvent);
}

void SceneItem::setItemOpacity(int currentOpacity) {
    setOpacity(currentOpacity/100.0);
}
