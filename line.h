#ifndef LINE_H
#define LINE_H

#include <QGraphicsLineItem>

class SceneItem;

class Line : public QGraphicsLineItem
{
public:
    Line(SceneItem *item1, SceneItem *item2);
    SceneItem *sender() {return mySender;}
    SceneItem *receiver() {return myReceiver;}
    void setState(bool on);
    bool activeSignal() {return active;}

private:
    bool bothItemsAreSenderAndReceiver;
    SceneItem *mySender, *myReceiver;
    bool active;
};

#endif // LINE_H
