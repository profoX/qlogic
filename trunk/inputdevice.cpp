#include "inputdevice.h"

#include <QTimeLine>

InputDevice::InputDevice(ItemType type, QMenu *contextMenu)
{
    myType = type;
    myContextMenu = contextMenu;
    mySignalType = SceneItem::Sender;
    outputSides = SceneItem::Left|SceneItem::Top|SceneItem::Right|SceneItem::Bottom;
    inputSides = None;
    maxGhostOpacity = 75;
    initItem();
}

void InputDevice::initAfterCreation() {
    SceneItem::initAfterCreation();
    if (myType == Oscillator) {
        oscillator = new QTimeLine(500, this);
        connect(oscillator, SIGNAL(finished()), this, SLOT(reverseState()));
        connect(oscillator, SIGNAL(finished()), oscillator, SLOT(start()));
        oscillator->start();
    }
}

void InputDevice::setSvgs() {
    QStringList svgs;
    switch (myType) {
        case Switch:
            svgs << ":/res/img/switch_on.svg" << ":/res/img/switch.svg";
            break;
        case Oscillator:
            svgs << ":/res/img/oscillator.svg" << ":/res/img/oscillator.svg";
            break;
        default:
            break;
    }
    SceneItem::changeSvg(svgs);
}
