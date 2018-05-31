#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiSubWindow>

class MdiChild;

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
    MdiChild* activeMdiChild();
    MdiChild* createMdiChild();
    QMdiSubWindow* findMdiChild(const QString &fileName);
    void setActiveSubWindow(QWidget* window);
private slots:
    void updateMenus();
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
};

#endif // MAINWINDOW_H
