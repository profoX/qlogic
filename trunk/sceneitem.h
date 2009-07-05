#ifndef SCENEITEM_H
#define SCENEITEM_H

#include <QGraphicsSvgItem>

class QTimeLine;

class SceneItem : public QGraphicsSvgItem
{
    Q_OBJECT

public:
    enum Type {Switch};
    SceneItem(Type type);
    int type() const {return myType;}

public slots:
    void deleteItem();

private slots:
    void setItemOpacity(int opacity);

private:
    Type myType;
    int currentOpacity;
    QTimeLine *timeLine;
};

#endif // SCENEITEM_H
