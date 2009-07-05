#include <QSlider>

#include "zoomwidget.h"
#include "ui_zoomwidget.h"

ZoomWidget::ZoomWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ZoomWidget)
{
    m_ui->setupUi(this);
}

ZoomWidget::~ZoomWidget()
{
    delete m_ui;
}

QSlider *ZoomWidget::slider() {
    return m_ui->slider;
}

void ZoomWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
