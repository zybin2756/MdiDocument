#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiSubWindow>

class MdiChild;
class QSignalMapper;
class QClipboard;
class QLabel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::MainWindow *ui;
    QSignalMapper *windowMapper;
    QClipboard* board;
    QLabel* RowColLabel;
    MdiChild* activeMdiChild();
    MdiChild* createMdiChild();
    QMdiSubWindow* findMdiChild(const QString &fileName);
    void writeSetting();
    void readSetting();
    void initWindow();

private slots:
    void updateMenus();
    void updateWindowMenu();
    void showTextRowAndCol();
    void setActiveSubWindow(QWidget* window);
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionClose_triggered();
    void on_actionCloseAll_triggered();
    void on_actionCut_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_actionNext_triggered();
    void on_actionPrevious_triggered();
    void on_actionTile_triggered();
    void on_actionCascade_triggered();
    void on_actionExit_triggered();
};

#endif // MAINWINDOW_H
