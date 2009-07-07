#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>

#include "sceneitem.h"

class QMenu;

class Scene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode {InsertItem, InsertLine, MoveItem};
    Scene(QMenu *menu, QObject *parent = 0);
    void setItemType(SceneItem::ItemType type) {itemType = type;}

public slots:
    void setMode(Mode mode);
    Mode mode() {return myMode;}
    void deleteItem();
    QGraphicsItem *collidingItemAt(QPointF pos);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
     SceneItem *item, *itemUnderLine;
     QGraphicsLineItem *line;
     Mode myMode;
     SceneItem::ItemType itemType;
     QMenu *itemMenu;
};

#endif // SCENE_H
