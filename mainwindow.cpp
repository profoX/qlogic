#include <QLabel>
#include <QSlider>
#include <QScrollBar>
#include <QGLWidget>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scene.h"
#include "zoomwidget.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    scene = new Scene(this);
    scene->setSceneRect(QRectF(0, 0, 10000, 10000));
    scene->setMode(Scene::InsertItem);
    scene->setItemType(SceneItem::Switch);
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

    viewStyleSheet = ui->view->styleSheet();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::unsetButtons() {
    ui->actionSelect->setChecked(false);
    ui->actionSwitch->setChecked(false);
    ui->actionLed->setChecked(false);
    ui->actionLine->setChecked(false);
    ui->actionAnd->setChecked(false);
    ui->actionOscillator->setChecked(false);
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
    scene->setItemType(SceneItem::Switch);
}

void MainWindow::on_actionLed_triggered()
{
    unsetButtons();
    ui->actionLed->setChecked(true);
    scene->setMode(Scene::InsertItem);
    scene->setItemType(SceneItem::Led);
}

void MainWindow::on_actionLine_triggered()
{
    unsetButtons();
    ui->actionLine->setChecked(true);
    scene->setMode(Scene::InsertLine);
}

void MainWindow::on_actionUseOpenGL_triggered(bool checked)
{
    if (checked)
        ui->view->setViewport(new QGLWidget);
    else {
        ui->view->setViewport(NULL);
        ui->view->setStyleSheet(viewStyleSheet);
    }
}

void MainWindow::on_actionAnd_triggered()
{
    unsetButtons();
    ui->actionAnd->setChecked(true);
    scene->setMode(Scene::InsertItem);
    scene->setItemType(SceneItem::AndGate);
}

void MainWindow::on_actionOscillator_triggered()
{
    unsetButtons();
    ui->actionOscillator->setChecked(true);
    scene->setMode(Scene::InsertItem);
    scene->setItemType(SceneItem::Oscillator);
}
