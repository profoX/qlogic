#include <QDebug>

#include <QSvgRenderer>
#include <QTimeLine>
#include <QGraphicsSceneMouseEvent>
#include <QBrush>
#include <QMenu>

#include "sceneitem.h"
#include "scene.h"
#include "line.h"

SceneItem::SceneItem(ItemType type, QMenu *contextMenu) {
    myType = type;
    myContextMenu = contextMenu;

    switch (myType) {
        case Switch:
        case Oscillator:
            mySignalType = Sender;
            break;
        case Led:
            mySignalType = Receiver;
            break;
        case AndGate:
        case NandGate:
            mySignalType = SenderAndReceiver;
            break;
        default:
            break;
    }

    on = false;
    ghost = true;
    setData(1, QString("Ghost"));
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

void SceneItem::initAfterCreation() {
    ghost = false;
    setData(1, QString("Solid"));
    if (myType == Oscillator) {
        oscillator = new QTimeLine(500, this);
        connect(oscillator, SIGNAL(finished()), this, SLOT(reverseState()));
        connect(oscillator, SIGNAL(finished()), oscillator, SLOT(start()));
        oscillator->start();
    }
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
    QListIterator<Line*> wires(attachedInWires << attachedOutWires);
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
        case Oscillator:
            svgs << ":/res/img/oscillator.svg" << ":/res/img/oscillator.svg";
            break;
        case Led:
            svgs << ":/res/img/led_on.svg" << ":/res/img/led.svg";
            break;
        case AndGate:
            svgs << ":/res/img/andgate.svg" << ":/res/img/andgate.svg";
            break;
        case NandGate:
            svgs << ":/res/img/nandgate.svg" << ":/res/img/nandgate.svg";
            break;
        default:
            break;
    }

    if (on)
        sharedRenderer = new QSvgRenderer(svgs[0], this);
    else
        sharedRenderer = new QSvgRenderer(svgs[1], this);
    qDebug() << "on: " << on << " svgs0 " << svgs[0] << " shared renderer " << sharedRenderer;

    setSharedRenderer(sharedRenderer);
}

void SceneItem::reverseState() {
    qDebug() << "reverseState()";
    on = !on;
    changeSvg();
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
    qDebug() << "updateSignalsOnWires()";
    while (wires.hasNext()) {
        Line *wire = wires.next();
        wire->setState(on);
    }
}

void SceneItem::processIncomingSignals() {
    QListIterator<Line*> wires(attachedInWires);
    bool outSignal;
    qDebug() << "processIncomingSignals()";
    switch (myType) {
        case AndGate:
        case NandGate:
            outSignal = true;
            break;
        default:
            outSignal = false;
            break;
    }

    while (wires.hasNext()) {
        Line *wire = wires.next();

        switch (myType) {
            case Led:
                if (wire->activeSignal())
                    outSignal = true;
                break;
            case AndGate:
            case NandGate:
                if (!wire->activeSignal())
                    outSignal = false;
                break;
            default:
                break;
        }
    }

    if (myType == NandGate)
        outSignal = !outSignal;

    on = outSignal;
    changeSvg();
    updateSignalsOnWires();
}

void SceneItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (!ghost) {
        scene()->clearSelection();
        setSelected(true);
        myContextMenu->exec(event->screenPos());
    }
}
