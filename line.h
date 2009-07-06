#ifndef LINE_H
#define LINE_H

#include <QGraphicsLineItem>

class SceneItem;

class Line : public QGraphicsLineItem
{
public:
    Line(SceneItem *item1, SceneItem *item2);

private:
    bool bothItemsAreSenderAndReceiver;
    SceneItem *sender, *receiver;
};

#endif // LINE_H
