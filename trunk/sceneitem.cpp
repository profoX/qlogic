#include <QDebug>

#include <QSvgRenderer>
#include <QTimeLine>

#include "sceneitem.h"

SceneItem::SceneItem(Type type) {
    myType = type;

    switch (type) {
        case Switch:
            QSvgRenderer *renderer = new QSvgRenderer(QString(":/res/img/switch.svg"), this);
            setSharedRenderer(renderer);
            break;
    }

    setOpacity(0.0);
    timeLine = new QTimeLine(333, this);
    timeLine->setFrameRange(0, 75);
    connect(timeLine, SIGNAL(frameChanged(int)), this, SLOT(setItemOpacity(int)));
    timeLine->start();
}

void SceneItem::deleteItem() {
    timeLine->setFrameRange(static_cast<int>(opacity() * 100.0), 0);
    connect(timeLine, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(timeLine, SIGNAL(frameChanged(int)), this, SLOT(setItemOpacity(int)));
    if (timeLine->state() == QTimeLine::Running)
        timeLine->stop();
    timeLine->start();
}

void SceneItem::setItemOpacity(int currentOpacity) {
    setOpacity(currentOpacity/100.0);
}
