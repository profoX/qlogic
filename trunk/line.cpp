#include <QDebug>

#include <QPen>
#include <QPainter>

#include "line.h"
#include "sceneitem.h"

Line::Line(const QLineF &line, QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsLineItem(line, parent, scene) {
    mySender = NULL;
    myReceiver = NULL;
    this->senderSide = SceneItem::None;
    this->receiverSide = SceneItem::None;
    setZValue(-1.0);
    setData(0, QVariant("Line"));
}

void Line::activate() {
    if (mySender && myReceiver) {
        mySender->attachOutWire(this);
        myReceiver->attachInWire(this);
        setState(mySender->outSignal());
    }
}

void Line::setItems(SceneItem *item1, SceneItem::Sides item1side, SceneItem *item2, SceneItem::Sides item2side, bool forceSenderOnItem2) {
    if (item1 && item2) {
        // Both can't be sender and both can't be receiver, otherwise this function would not be called
        if (item1->signalType() == SceneItem::Sender || item2->signalType() == SceneItem::Receiver) {
            mySender = item1;
            myReceiver = item2;
            senderSide = item1side;
            receiverSide = item2side;
        } else if (item2->signalType() == SceneItem::Sender || item1->signalType() == SceneItem::Receiver || forceSenderOnItem2) {
            mySender = item2;
            myReceiver = item1;
            senderSide = item2side;
            receiverSide = item1side;
        } else {
            mySender = item1;
            myReceiver = item2;
            senderSide = item1side;
            receiverSide = item2side;
        }
    } else {
        mySender = NULL;
        myReceiver = NULL;
        senderSide = SceneItem::None;
        receiverSide = SceneItem::None;
    }
}

void Line::setSides(SceneItem::Sides senderSide, SceneItem::Sides receiverSide) {
    this->senderSide = senderSide;
    this->receiverSide = receiverSide;
}

void Line::setState(bool on) {
    active = on;
    if (active) {
        setPen(QPen(Qt::red, 1));
    } else {
        setPen(QPen(Qt::blue, 1));
    }
    myReceiver->processIncomingSignals();
}

void Line::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    // Draw extra lines
    QPointF p1 = line().p1(), p2 = line().p2();
    painter->setPen(pen());
    if (mySender && myReceiver) {
        switch (senderSide) {
            case SceneItem::Right:
                p1 = QPointF(mySender->x() + mySender->boundingRect().width() + 5.5,
                             mySender->y() + mySender->boundingRect().center().y());
                p2 = QPointF(myReceiver->x() - 5.5,
                             myReceiver->y() + myReceiver->boundingRect().center().y());
                break;
            case SceneItem::Left:
                p1 = QPointF(mySender->x() - 5.5,
                             mySender->y() + mySender->boundingRect().center().y());
                p2 = QPointF(myReceiver->x() + myReceiver->boundingRect().width() + 5.5,
                             myReceiver->y() + myReceiver->boundingRect().center().y());
                break;
            case SceneItem::Bottom:
                p1 = QPointF(mySender->x() + mySender->boundingRect().center().x(),
                             mySender->y() + mySender->boundingRect().height() + 5.5);
                p2 = QPointF(myReceiver->x() + myReceiver->boundingRect().center().x(),
                             myReceiver->y() - 5.5);
                break;
            case SceneItem::Top:
                p1 = QPointF(mySender->x() + mySender->boundingRect().center().x(),
                             mySender->y() - 5.5);
                p2 = QPointF(myReceiver->x() + myReceiver->boundingRect().center().x(),
                             myReceiver->y() + myReceiver->boundingRect().height() + 5.5);
                break;
            default:
                break;
        }
        painter->drawLine(mySender->pos() + mySender->boundingRect().center(), p1);
        painter->drawLine(myReceiver->pos() + myReceiver->boundingRect().center(), p2);
    }
    painter->drawLine(p1, p2);
}

QList<SceneItem::Sides> Line::shortestPossibleWire(SceneItem *source, SceneItem *target, BubbleItem::BubbleIcon icon) {
    QList<SceneItem::Sides> wire;

    // A valid wire can only be drawn when one item can act as sender and the other as receiver
    SceneItem::SignalType validSignalTypes = validSignalTypesFromTo(source, target);
    if (validSignalTypes == SceneItem::Invalid || validSignalTypes == SceneItem::Locked)
        return wire;

    // Valid sides for items
    int validSidesTarget = (icon == BubbleItem::Input ? target->inputSides() : target->outputSides());
    int validSidesSource = (icon == BubbleItem::Input ? source->outputSides() : source->inputSides());

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

SceneItem::SignalType Line::validSignalTypesFromTo(SceneItem *from, SceneItem *to) {
    if ((from->signalType() == to->signalType() && to->signalType() != SceneItem::SenderAndReceiver))
        return SceneItem::Invalid;
    else
        return to->signalType();
}
