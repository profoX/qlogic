#include "outputdevice.h"
#include "line.h"

#include <QDebug>

OutputDevice::OutputDevice(ItemType type, QMenu *contextMenu)
{
    myType = type;
    myContextMenu = contextMenu;
    mySignalType = SceneItem::Receiver;
    initItem();
}

void OutputDevice::processIncomingSignals() {
    QListIterator<Line*> wires(attachedInWires);
    bool outSignal = false;
    qDebug() << "processIncomingSignals()" << myType;

    while (wires.hasNext()) {
        Line *wire = wires.next();

        switch (myType) {
            case Led:
                if (wire->activeSignal())
                    outSignal = true;
                break;
            default:
                break;
        }
    }

    on = outSignal;
    setSvgs();
}

void OutputDevice::setSvgs() {
    QStringList svgs;
    switch (myType) {
        case Led:
            svgs << ":/res/img/led_on.svg" << ":/res/img/led.svg";
            break;
        default:
            break;
    }
    SceneItem::changeSvg(svgs);
}
