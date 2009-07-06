#ifndef SCENEITEM_H
#define SCENEITEM_H

#include <QGraphicsSvgItem>

class QTimeLine;
class Line;

class SceneItem : public QGraphicsSvgItem
{
    Q_OBJECT

public:
    enum ItemType {Switch, Led};
    enum SignalType {Sender, Receiver, SenderAndReceiver};
    enum {Type = UserType + 1};

    SceneItem(ItemType type);
    int type() const {return Type;}
    ItemType itemType() const {return myType;}
    SignalType signalType() const {return mySignalType;}
    void showOverlayItem();
    void hideOverlayItem();
    bool checkCollision();
    void attachWire(Line *wire);
    void lockOpacity(qreal opacity);

public slots:
    void deleteItem();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private slots:
    void setItemOpacity(int opacity);

private:
    void moveWithWires(QPointF newPosition);
    void changeSvg();
    ItemType myType;
    SignalType mySignalType;
    int currentOpacity;
    QTimeLine *timeLine;
    bool on;
    QGraphicsRectItem *overlayItem;
    QPointF oldPosition;
    QList<Line*> attachedWires;
};

#endif // SCENEITEM_H
