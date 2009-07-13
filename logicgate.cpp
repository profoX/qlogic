#include "logicgate.h"
#include "line.h"

#include <QDebug>

LogicGate::LogicGate(ItemType type, QMenu *contextMenu) {
    myType = type;
    myContextMenu = contextMenu;
    mySignalType = SceneItem::SenderAndReceiver;
    myInputSides = SceneItem::Left;
    myOutputSides = SceneItem::Right;
    inverted = (myType == NandGate || myType == NorGate);
    maxGhostOpacity = 75;
    initItem();

    switch (myType) {
        case AndGate:
        case NandGate:
        case OrGate:
        case NorGate:
            maxIn = 2;
            break;
        case NotGate:
            maxIn = 1;
            break;
        default:
            break;
    }

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
        case OrGate:
        case NorGate:
            outSignal = false;
            break;
        case NotGate:
            outSignal = true;
            break;
        default:
            outSignal = false;
            break;
    }

    int c = 0;
    while (wires.hasNext()) {
        Line *wire = wires.next();
        ++c;
        qDebug() << "wireCount " << c;

        switch (myType) {
            case AndGate:
            case NandGate:
                if (!wire->activeSignal())
                    outSignal = false;
                break;
            case OrGate:
            case NorGate:
                if (wire->activeSignal())
                    outSignal = true;
                break;
            case NotGate:
                if (wire->activeSignal())
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
        case SceneItem::OrGate:
            svgs << ":/res/img/orgate.svg" << ":/res/img/orgate.svg";
            break;
        case SceneItem::NorGate:
            svgs << ":/res/img/norgate.svg" << ":/res/img/norgate.svg";
            break;
        case SceneItem::NotGate:
            svgs << ":/res/img/notgate.svg" << ":/res/img/notgate.svg";
            break;
        default:
            break;
    }
    SceneItem::changeSvg(svgs);
}
