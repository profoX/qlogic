#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>

class SceneItem;

class Scene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode {InsertItem, InsertLine, InsertText, MoveItem};
    Scene(QObject *parent = 0);

public slots:
    void setMode(Mode mode) {this->mode = mode;}
    void deleteItem();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    //void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
     SceneItem *item;
     QGraphicsRectItem *overlayItem;
     Mode mode;
};

#endif // SCENE_H
