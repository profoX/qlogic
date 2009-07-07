#include "logicgate.h"
#include "line.h"

#include <QDebug>

LogicGate::LogicGate(ItemType type, QMenu *contextMenu)
{
    myType = type;
    myContextMenu = contextMenu;
    mySignalType = SceneItem::SenderAndReceiver;
    inputSides = SceneItem::Left;
    outputSides = SceneItem::Right;
    inverted = (myType == NandGate);
    maxGhostOpacity = 75;
    initItem();
}

void LogicGate::processIncomingSignals() {
    QListIterator<Line*> wires(attachedInWires);
    bool outSignal;
    qDebug() << "processIncomingSignals()" << myType;

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
            case AndGate:
            case NandGate:
                if (!wire->activeSignal())
                    outSignal = false;
                break;
            default:
                break;
        }
    }

    if (inverted)
        outSignal = !outSignal;

    if (on != outSignal) {
        on = outSignal;
        setSvgs();
        updateSignalsOnWires();
    }
}

void LogicGate::setSvgs() {
    QStringList svgs;
    switch (myType) {
        case SceneItem::AndGate:
            svgs << ":/res/img/andgate.svg" << ":/res/img/andgate.svg";
            break;
        case SceneItem::NandGate:
            svgs << ":/res/img/nandgate.svg" << ":/res/img/nandgate.svg";
            break;
        default:
            break;
    }
    SceneItem::changeSvg(svgs);
}
