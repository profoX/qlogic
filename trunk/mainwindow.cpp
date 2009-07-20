#include <QDebug>

#include <QLabel>
#include <QSlider>
#include <QScrollBar>
#include <QButtonGroup>
#include <QPushButton>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scene.h"
#include "zoomwidget.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    createScene();
    //QWidget *spacer = new QWidget(ui->toolBar);
    //spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //ZoomWidget *zoomWidget = new ZoomWidget(ui->toolBar);
    //ui->toolBar->addWidget(spacer);
    //ui->toolBar->addWidget(zoomWidget);
    //connect(zoomWidget->slider(), SIGNAL(valueChanged(int)), ui->view, SLOT(zoom(int)));
    //ui->view->zoom(zoomWidget->slider()->value());
    ui->view->zoom(10);
    ui->view->horizontalScrollBar()->setValue(0);
    ui->view->verticalScrollBar()->setValue(0);
    buildActionList();
    on_actionSwitch_triggered();
    connect(actionButtons, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(changeAction(QAbstractButton*)));
}

void MainWindow::createActionButton(QAction *action, int type) {
    QPushButton *button = new QPushButton(action->icon(), action->text(), ui->scrollArea);
    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    button->setIconSize(QSize(32, 32));
    button->addAction(action);
    button->setCheckable(true);
    button->setStyleSheet(QString("text-align: left; font-weight: bold;"));
    actionButtons->addButton(button, type);
    ui->scrollArea->widget()->layout()->addWidget(button);
    button->resize(button->sizeHint());
}

void MainWindow::buildActionList() {
    QVBoxLayout *layout = new QVBoxLayout;
    QWidget *widget = new QWidget(ui->scrollArea);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    widget->setLayout(layout);
    ui->scrollArea->setWidget(widget);

    actionButtons = new QButtonGroup(this);
    createActionButton(ui->actionSelect, SelectMode);
    createActionButton(ui->actionLine, LineMode);
    createActionButton(ui->actionAnd, AndGate);
    createActionButton(ui->actionNand, NandGate);
    createActionButton(ui->actionOr, OrGate);
    createActionButton(ui->actionNor, NorGate);
    createActionButton(ui->actionNot, NotGate);
    createActionButton(ui->actionSwitch, SwitchItem);
    createActionButton(ui->actionOscillator, OscillatorItem);
    createActionButton(ui->actionLed, LedItem);
    layout->addStretch();

    ui->scrollArea->setMinimumWidth(ui->scrollArea->widget()->childrenRect().width() + style()->pixelMetric(QStyle::PM_ScrollBarExtent) + 4);
}

void MainWindow::createScene() {
    removeItem = new QAction(tr("Remove item (but keep wires)"), this);
    removeItemWithWires = new QAction(tr("Remove item (and remove wires)"), this);
    itemMenu = new QMenu(this);
    itemMenu->addAction(removeItem);
    itemMenu->addAction(removeItemWithWires);

    scene = new Scene(itemMenu, this);
    scene->setSceneRect(QRectF(0, 0, 10000, 10000));
    scene->setMode(Scene::InsertItem);
    scene->setItemType(SceneItem::Switch);
    ui->view->setScene(scene);
    connect(ui->view, SIGNAL(mouseLeftView()), scene, SLOT(deleteItem()));
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
    ui->actionNand->setChecked(false);
    ui->actionOr->setChecked(false);
    ui->actionNor->setChecked(false);
    ui->actionNot->setChecked(false);
}

void MainWindow::on_actionSelect_triggered()
{
    unsetButtons();
    ui->actionSelect->setChecked(true);
    actionButtons->button(SelectMode)->setChecked(true);
    scene->setMode(Scene::MoveItem);
}

void MainWindow::on_actionLine_triggered()
{
    unsetButtons();
    ui->actionLine->setChecked(true);
    actionButtons->button(LineMode)->setChecked(true);
    scene->setMode(Scene::InsertLine);
}

void MainWindow::on_actionSwitch_triggered()
{
    unsetButtons();
    ui->actionSwitch->setChecked(true);
    actionButtons->button(SwitchItem)->setChecked(true);
    scene->setMode(Scene::InsertItem);
    scene->setItemType(SceneItem::Switch);
}

void MainWindow::on_actionLed_triggered()
{
    unsetButtons();
    ui->actionLed->setChecked(true);
    actionButtons->button(LedItem)->setChecked(true);
    scene->setMode(Scene::InsertItem);
    scene->setItemType(SceneItem::Led);
}

void MainWindow::on_actionAnd_triggered()
{
    unsetButtons();
    ui->actionAnd->setChecked(true);
    actionButtons->button(AndGate)->setChecked(true);
    scene->setMode(Scene::InsertItem);
    scene->setItemType(SceneItem::AndGate);
}

void MainWindow::on_actionOscillator_triggered()
{
    unsetButtons();
    ui->actionOscillator->setChecked(true);
    actionButtons->button(OscillatorItem)->setChecked(true);
    scene->setMode(Scene::InsertItem);
    scene->setItemType(SceneItem::Oscillator);
}

void MainWindow::on_actionNand_triggered()
{
    unsetButtons();
    ui->actionNand->setChecked(true);
    actionButtons->button(NandGate)->setChecked(true);
    scene->setMode(Scene::InsertItem);
    scene->setItemType(SceneItem::NandGate);
}

void MainWindow::on_actionOr_triggered()
{
    unsetButtons();
    ui->actionOr->setChecked(true);
    actionButtons->button(OrGate)->setChecked(true);
    scene->setMode(Scene::InsertItem);
    scene->setItemType(SceneItem::OrGate);
}

void MainWindow::on_actionNor_triggered()
{
    unsetButtons();
    ui->actionNor->setChecked(true);
    actionButtons->button(NorGate)->setChecked(true);
    scene->setMode(Scene::InsertItem);
    scene->setItemType(SceneItem::NorGate);
}

void MainWindow::on_actionNot_triggered()
{
    unsetButtons();
    ui->actionNot->setChecked(true);
    actionButtons->button(NotGate)->setChecked(true);
    scene->setMode(Scene::InsertItem);
    scene->setItemType(SceneItem::NotGate);
}

void MainWindow::changeAction(QAbstractButton* button) {
    button->actions().first()->trigger();
}
