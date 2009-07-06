#ifndef SCENEITEM_H
#define SCENEITEM_H

#include <QGraphicsSvgItem>

class QTimeLine;

class SceneItem : public QGraphicsSvgItem
{
    Q_OBJECT

public:
    enum Type {Switch, Led};
    SceneItem(Type type);
    int type() const {return myType;}
    void showOverlayItem();
    void hideOverlayItem();
    bool checkCollision();

public slots:
    void deleteItem();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private slots:
    void setItemOpacity(int opacity);

private:
    void changeSvg();
    Type myType;
    int currentOpacity;
    QTimeLine *timeLine;
    bool on;
    QGraphicsRectItem *overlayItem;
    QPointF oldPosition;
};

#endif // SCENEITEM_H
