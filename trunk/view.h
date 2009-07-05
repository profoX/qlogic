#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>

class View : public QGraphicsView
{
    Q_OBJECT

public:
    View(QWidget *parent = 0);

public slots:
    void zoom(int zoomLevel);

protected:
    void leaveEvent (QEvent *event);

signals:
    void mouseLeftView();
};

#endif // VIEW_H
