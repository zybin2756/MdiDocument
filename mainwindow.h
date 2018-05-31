#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiSubWindow>

class MdiChild;
class QSignalMapper;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSignalMapper *windowMapper;
    MdiChild* activeMdiChild();
    MdiChild* createMdiChild();
    QMdiSubWindow* findMdiChild(const QString &fileName);


private slots:
    void updateMenus();
    void updateWindowMenu();
    void setActiveSubWindow(QWidget* window);
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
};

#endif // MAINWINDOW_H
