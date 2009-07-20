#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class Scene;
class QSlider;
class QButtonGroup;
class QAbstractButton;
class QAction;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum ActionType {SelectMode = 0, LineMode = 1,
                     AndGate = 2, NandGate = 3, OrGate = 4, NorGate = 5, NotGate = 6,
                     SwitchItem = 7, OscillatorItem = 8, LedItem = 9};
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void createActionButton(QAction *action, int type);
    void buildActionList();
    void unsetButtons();
    void createScene();
    Ui::MainWindow *ui;
    Scene *scene;
    //QSlider *slider;
    QButtonGroup *actionButtons;

    QAction *removeItem;
    QAction *removeItemWithWires;
    QMenu *itemMenu;

private slots:
    void changeAction(QAbstractButton *button);
    void on_actionNot_triggered();
    void on_actionNor_triggered();
    void on_actionOr_triggered();
    void on_actionNand_triggered();
    void on_actionOscillator_triggered();
    void on_actionAnd_triggered();
    void on_actionLine_triggered();
    void on_actionLed_triggered();
    void on_actionSwitch_triggered();
    void on_actionSelect_triggered();
};

#endif // MAINWINDOW_H
