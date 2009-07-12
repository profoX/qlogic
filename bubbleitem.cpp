#include <QDebug>

#include "bubbleitem.h"

BubbleItem::BubbleItem(BubbleIcon icon) {
    myType = SceneItem::Bubble;
    mySignalType = SceneItem::Invalid;
    myIcon = icon;
    maxGhostOpacity = 50;
    initItem();
    setData(0, QVariant("Bubble"));
}

void BubbleItem::setSvgs() {
    QStringList svgs;
    switch (myIcon) {
        case Invalid:
            svgs << ":/res/img/error.svg" << ":/res/img/error.svg";
            break;
        case Input:
            svgs << ":/res/img/input.svg" << ":/res/img/input.svg";
            break;
        case Output:
            svgs << ":/res/img/output.svg" << ":/res/img/output.svg";
            break;
    }
    SceneItem::changeSvg(svgs);
}
