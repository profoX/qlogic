#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class Scene;
class QSlider;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void unsetButtons();
    void createScene();
    Ui::MainWindow *ui;
    Scene *scene;
    QSlider *slider;
    QString viewStyleSheet;

    QAction *removeItem;
    QAction *removeItemWithWires;
    QMenu *itemMenu;

private slots:
    void on_actionNot_triggered();
    void on_actionNor_triggered();
    void on_actionOr_triggered();
    void on_actionNand_triggered();
    void on_actionOscillator_triggered();
    void on_actionAnd_triggered();
    void on_actionUseOpenGL_triggered(bool checked);
    void on_actionLine_triggered();
    void on_actionLed_triggered();
    void on_actionSwitch_triggered();
    void on_actionSelect_triggered();
};

#endif // MAINWINDOW_H
