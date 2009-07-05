#ifndef ZOOMWIDGET_H
#define ZOOMWIDGET_H

#include <QtGui/QWidget>

class QSlider;

namespace Ui {
    class ZoomWidget;
}

class ZoomWidget : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(ZoomWidget)
public:
    explicit ZoomWidget(QWidget *parent = 0);
    virtual ~ZoomWidget();
    QSlider *slider();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::ZoomWidget *m_ui;
};

#endif // ZOOMWIDGET_H
