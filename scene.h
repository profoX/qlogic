#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>

#include "sceneitem.h"

class Scene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode {InsertItem, InsertLine, InsertText, MoveItem};
    Scene(QObject *parent = 0);
    void setItemType(SceneItem::Type type) {itemType = type;}

public slots:
    void setMode(Mode mode) {myMode = mode;}
    Mode mode() {return myMode;}
    void deleteItem();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
     SceneItem *item;
     Mode myMode;
     SceneItem::Type itemType;
};

#endif // SCENE_H
