#include <QDebug>

#include <QSvgRenderer>
#include <QTimeLine>
#include <QGraphicsSceneMouseEvent>
#include <QBrush>
#include <QMenu>

#include "sceneitem.h"
#include "scene.h"
#include "line.h"

void SceneItem::initItem() {
    on = false;
    ghost = true;
    setData(1, QString("Ghost"));
    overlayItem = new QGraphicsRectItem(this);
    setFlags(QGraphicsItem::ItemDoesntPropagateOpacityToChildren | QGraphicsItem::ItemIsMovable);
    setSvgs();
    setData(0, QVariant("Item"));

    setOpacity(0.0);
    timeLine = new QTimeLine(333, this);
    timeLine->setFrameRange(0, maxGhostOpacity);
    connect(timeLine, SIGNAL(frameChanged(int)), this, SLOT(setItemOpacity(int)));
    timeLine->start();
}

void SceneItem::initAfterCreation() {
    ghost = false;
    setData(1, QString("Solid"));
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
            if (collidingItem != overlayItem && collidingItem->data(0) != "Line" && collidingItem->data(1) == "Solid") {
                showOverlayItem();
                return true;
            }
        }
        hideOverlayItem();
        return false;
    }
}

void SceneItem::attachInWire(Line *wire) {
    attachedInWires << wire;
}

void SceneItem::attachOutWire(Line *wire) {
    attachedOutWires << wire;
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
    Scene::Mode mode = qobject_cast<Scene*>(scene())->mode();
    if (mode == Scene::MoveItem) {
        QPointF newPosition(static_cast<int>(mouseEvent->scenePos().x() - static_cast<int>(mouseEvent->scenePos().x()) % 50),
                            static_cast<int>(mouseEvent->scenePos().y() - static_cast<int>(mouseEvent->scenePos().y()) % 50));
        moveWithWires(newPosition);
        checkCollision();
    }
}

void SceneItem::moveWithWires(QPointF newPosition) {
    setPos(newPosition);

    // Move attached wires as well
    QListIterator<Line*> wires(attachedInWires << attachedOutWires);
    while (wires.hasNext()) {
        Line *wire = wires.next();
        wire->setLine(QLineF(wire->sender()->pos() + wire->sender()->boundingRect().center(),
                             wire->receiver()->pos() + wire->receiver()->boundingRect().center()));
    }
}

void SceneItem::changeSvg(const QStringList &svgs) {    
    QSvgRenderer *sharedRenderer;
    renderer()->deleteLater();
    sharedRenderer = new QSvgRenderer(on ? svgs[0] : svgs[1], this);
    qDebug() << "on: " << on << " svgs[0]: " << svgs[0] << " shared renderer: " << sharedRenderer;
    setSharedRenderer(sharedRenderer);
}

void SceneItem::reverseState() {
    qDebug() << "reverseState()";
    on = !on;
    setSvgs();
    updateSignalsOnWires();
}

void SceneItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (qobject_cast<Scene*>(scene())->mode() == Scene::MoveItem) {
        if (checkCollision()) {
            moveWithWires(oldPosition);
            hideOverlayItem();
        } else if (myType == Switch && oldPosition == scenePos()) {
            reverseState();
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

void SceneItem::updateSignalsOnWires() {
    QListIterator<Line*> wires(attachedOutWires);
    qDebug() << "updateSignalsOnWires()" << myType;
    while (wires.hasNext()) {
        Line *wire = wires.next();
        wire->setState(on);
    }
}

void SceneItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (!ghost) {
        scene()->clearSelection();
        setSelected(true);
        myContextMenu->exec(event->screenPos());
    }
}

QList<SceneItem*> SceneItem::attachedDevices() {
    QList<SceneItem *> devices;
    QListIterator<Line*> inWires(attachedInWires);
    QListIterator<Line*> outWires(attachedOutWires);

    while (inWires.hasNext())
        devices << inWires.next()->sender();
    while (outWires.hasNext())
        devices << outWires.next()->receiver();

    return devices;
}
