#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mdichild.h"
#include <QFileDialog>

#include "mdichild.h"

#include <QMdiSubWindow>
#include <QFIleInfo>
#include <QFileDialog>
#include <QSignalMapper>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->windowMapper = new QSignalMapper(this);

    updateMenus();
    updateWindowMenu();
    connect(ui->menu_W,SIGNAL(aboutToShow()),this,SLOT(updateWindowMenu()));
    connect(windowMapper,SIGNAL(mapped(QWidget*)),this,SLOT(setActiveSubWindow(QWidget*)));
    connect(ui->mdiArea,SIGNAL(subWindowActivated(QMdiSubWindow*)),this,SLOT(updateMenus()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//更新工具栏状态
void MainWindow::updateMenus(){
    bool hasMdiChild = (activeMdiChild() != 0);
    ui->actionSave->setEnabled(hasMdiChild);
    ui->actionSaveAs->setEnabled(hasMdiChild);
    ui->actionTile->setEnabled(hasMdiChild);
    ui->actionCascade->setEnabled(hasMdiChild);
    ui->actionPaste->setEnabled(hasMdiChild);
    ui->actionNext->setEnabled(hasMdiChild);
    ui->actionPrevious->setEnabled(hasMdiChild);
    ui->actionClose->setEnabled(hasMdiChild);
    ui->actionCloseAll->setEnabled(hasMdiChild);

    bool hasSelection = (activeMdiChild()
                         && activeMdiChild()->textCursor().hasSelection());
    ui->actionCut->setEnabled(hasSelection);
    ui->actionCopy->setEnabled(hasSelection);

    ui->actionUndo->setEnabled(activeMdiChild()
                               && activeMdiChild()->document()->isUndoAvailable());

    ui->actionRedo->setEnabled(activeMdiChild()
                               && activeMdiChild()->document()->isRedoAvailable());
}

//更新窗口菜单项
void MainWindow::updateWindowMenu(){
    ui->menu_W->clear();
    ui->menu_W->addAction(ui->actionClose);
    ui->menu_W->addAction(ui->actionCloseAll);
    ui->menu_W->addSeparator();
    ui->menu_W->addAction(ui->actionTile);
    ui->menu_W->addAction(ui->actionCascade);
    ui->menu_W->addSeparator();
    ui->menu_W->addAction(ui->actionNext);
    ui->menu_W->addAction(ui->actionPrevious);
    QList<QMdiSubWindow*> windows = ui->mdiArea->subWindowList();
    if(!windows.isEmpty())
        ui->menu_W->addSeparator();


    for(int i = 0; i < windows.size(); ++i){
        MdiChild *child = qobject_cast<MdiChild*>(windows.at(i)->widget());
        QString text;
        if(i < 9){
            text = tr("&%1 %2").arg(i+1).arg(child->userFirendlyName());
        }else{
            text = tr("%1 %2").arg(i+1).arg(child->userFirendlyName());
        }


        QAction *action = ui->menu_W->addAction(text);
        action->setCheckable(true);
        action->setChecked(child == activeMdiChild());
        connect(action,SIGNAL(triggered()),this->windowMapper,SLOT(map()));
        this->windowMapper->setMapping(action, windows.at(i));
    }
}

//获取当前子窗口
MdiChild* MainWindow::activeMdiChild(){
    if(QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
        return qobject_cast<MdiChild*>(activeSubWindow->widget());
    return 0;
}

//创建子窗口
MdiChild* MainWindow::createMdiChild(){
    MdiChild* child = new MdiChild;
    ui->mdiArea->addSubWindow(child);

    connect(child, SIGNAL(copyAvailable(bool)),ui->actionCopy,SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),ui->actionCut,SLOT(setEnabled(bool)));

    connect(child->document(), SIGNAL(undoAvailable(bool)),ui->actionUndo,SLOT(setEnabled(bool)));
    connect(child->document(), SIGNAL(redoAvailable(bool)),ui->actionRedo,SLOT(setEnabled(bool)));

    return child;
}

//根据文件名查找子窗口
QMdiSubWindow* MainWindow::findMdiChild(const QString &fileName){
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
    foreach(QMdiSubWindow *window, ui->mdiArea->subWindowList()){
        MdiChild* child = qobject_cast<MdiChild*>(window->widget());
        if(child->currentFile() == canonicalFilePath){
            return window;
        }
    }
    return 0;
}

//设置活动子窗口
void MainWindow::setActiveSubWindow(QWidget* window){
    if(!window)
        return;
    ui->mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(window));
}

//新建文件
void MainWindow::on_actionNew_triggered()
{
    MdiChild* child = this->createMdiChild();
    child->newFile();
    child->show();
}

//打开文件
void MainWindow::on_actionOpen_triggered()
{

    QString fileName = QFileDialog::getOpenFileName(this);
    if(fileName.isEmpty()){
        return;
    }

    if( !fileName.isEmpty()){
        QMdiSubWindow* window = findMdiChild(fileName);
        if(window){
            setActiveSubWindow(window);
            return;
        }
    }

    MdiChild* child = createMdiChild();
    if(child->loadFile(fileName)){
        ui->statusBar->showMessage(tr("打开文件成功"),2000);
        child->show();
    }else{
        child->close();
    }
}

