#ifndef SCENEITEM_H
#define SCENEITEM_H

#include <QGraphicsSvgItem>

class QTimeLine;
class Line;
class QMenu;

class SceneItem : public QGraphicsSvgItem
{
    Q_OBJECT

public:
    enum ItemType {Switch, Oscillator, Led, AndGate, NandGate};
    enum SignalType {Sender, Receiver, SenderAndReceiver};
    enum {Type = UserType + 1};

    SceneItem(ItemType type, QMenu *contextMenu);
    void initAfterCreation();
    int type() const {return Type;}
    ItemType itemType() const {return myType;}
    SignalType signalType() const {return mySignalType;}
    void showOverlayItem();
    void hideOverlayItem();
    bool checkCollision();
    void attachInWire(Line *wire);
    void attachOutWire(Line *wire);
    void lockOpacity(qreal opacity);
    void updateSignalsOnWires();
    bool outSignal() {return on;}
    void processIncomingSignals();

public slots:
    void deleteItem();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private slots:
    void setItemOpacity(int opacity);
    void reverseState();

private:
    void moveWithWires(QPointF newPosition);
    void changeSvg();

    ItemType myType;
    SignalType mySignalType;
    int currentOpacity;
    QTimeLine *timeLine, *oscillator;
    bool on;
    QGraphicsRectItem *overlayItem;
    QPointF oldPosition;
    QList<Line*> attachedInWires;
    QList<Line*> attachedOutWires;
    QMenu *myContextMenu;
    bool ghost;
};

#endif // SCENEITEM_H
