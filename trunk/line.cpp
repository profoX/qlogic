#include <QDebug>

#include <QPen>

#include "line.h"
#include "sceneitem.h"

Line::Line(SceneItem *item1, SceneItem *item2)
{
    bothItemsAreSenderAndReceiver = item1->signalType() == SceneItem::SenderAndReceiver && item2->signalType() == SceneItem::SenderAndReceiver;

    // Both can't be sender and both can't be receiver, otherwise this function would not be called
    if (item1->signalType() == SceneItem::Sender || item2->signalType() == SceneItem::Receiver) {
        mySender = item1;
        myReceiver = item2;
    } else if (item2->signalType() == SceneItem::Sender || item1->signalType() == SceneItem::Receiver) {
        mySender = item2;
        myReceiver = item1;
    } else {
        mySender = item1;
        myReceiver = item2;
    }

    mySender->attachOutWire(this);
    myReceiver->attachInWire(this);
    setState(mySender->outSignal());

    setZValue(-1.0);
    setData(0, QVariant("Line"));
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
