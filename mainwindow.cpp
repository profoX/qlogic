#include <QLabel>
#include <QSlider>
#include <QScrollBar>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scene.h"
#include "zoomwidget.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    scene = new Scene(this);
    scene->setSceneRect(QRectF(0, 0, 10000, 10000));
    scene->setMode(Scene::InsertItem);
    ui->view->setScene(scene);
    connect(ui->view, SIGNAL(mouseLeftView()), scene, SLOT(deleteItem()));

    QWidget *spacer = new QWidget(ui->toolBar);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ZoomWidget *zoomWidget = new ZoomWidget(ui->toolBar);
    ui->toolBar->addWidget(spacer);
    ui->toolBar->addWidget(zoomWidget);
    connect(zoomWidget->slider(), SIGNAL(valueChanged(int)), ui->view, SLOT(zoom(int)));
    ui->view->zoom(zoomWidget->slider()->value());

    ui->view->horizontalScrollBar()->setValue(0);
    ui->view->verticalScrollBar()->setValue(0);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::unsetButtons() {
    ui->actionSelect->setChecked(false);
    ui->actionSwitch->setChecked(false);
}

void MainWindow::on_actionSelect_triggered()
{
    unsetButtons();
    ui->actionSelect->setChecked(true);
    scene->setMode(Scene::MoveItem);
}

void MainWindow::on_actionSwitch_triggered()
{
    unsetButtons();
    ui->actionSwitch->setChecked(true);
    scene->setMode(Scene::InsertItem);
}
