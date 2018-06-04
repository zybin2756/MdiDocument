#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mdichild.h"
#include "hightlighter.h"

#include <QDebug>
#include <QFIleInfo>
#include <QSignalMapper>
#include <QClipboard>
#include <QSettings>

#include <QFileDialog>
#include <QMdiSubWindow>
#include <QLabel>

#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initWindow();

    updateMenus();
    updateWindowMenu();
    readSetting();
    connect(ui->menu_W,SIGNAL(aboutToShow()),this,SLOT(updateWindowMenu()));
    connect(windowMapper,SIGNAL(mapped(QWidget*)),this,SLOT(setActiveSubWindow(QWidget*)));
    connect(ui->mdiArea,SIGNAL(subWindowActivated(QMdiSubWindow*)),this,SLOT(updateMenus()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initWindow(){
    ui->mdiArea->setViewMode(QMdiArea::TabbedView);
    ui->mdiArea->setTabsClosable(true);
    ui->mdiArea->setTabsMovable(true);
    ui->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mainToolBar->setWindowTitle(tr("工具栏"));

    this->windowMapper = new QSignalMapper(this);
    this->board = QApplication::clipboard();

    this->RowColLabel = new QLabel;
    this->RowColLabel->setFrameStyle(QFrame::Box | QFrame::Sunken);
    ui->statusBar->addPermanentWidget(RowColLabel);

    ui->actionNew->setStatusTip(tr("新建文件"));
    ui->actionSave->setStatusTip(tr("保存文件"));
    ui->actionSaveAs->setStatusTip(tr("另存为"));
    ui->actionOpen->setStatusTip(tr("打开文件"));
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
    ui->actionFind->setEnabled(hasMdiChild);
    ui->actionReplace->setEnabled(hasMdiChild);
    ui->actionGoto->setEnabled(hasMdiChild);
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

void MainWindow::showTextRowAndCol(){
    if(activeMdiChild()){
        int rowNum = activeMdiChild()->textCursor().blockNumber() + 1;
        int colNum = activeMdiChild()->textCursor().columnNumber() + 1;

        RowColLabel->setText(tr("%1行 %2列").arg(rowNum).arg(colNum));
//        ui->statusBar->showMessage(tr("%1行 %2列").arg(rowNum).arg(colNum),2000);
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
    child->highLighter = new Highlighter(child->document());
    child->setMouseTracking(true);
    ui->mdiArea->addSubWindow(child);
    connect(child, SIGNAL(copyAvailable(bool)),ui->actionCopy,SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),ui->actionCut,SLOT(setEnabled(bool)));
    connect(child, SIGNAL(cursorPositionChanged()),this,SLOT(showTextRowAndCol()));
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

void MainWindow::writeSetting(){
    QSettings settings("zyb","MdiDocu");
    settings.setValue("pos",pos());
    settings.setValue("size",size());
}

void MainWindow::readSetting(){
    QSettings settings("zyb","MdiDocu");
    QPoint pos = settings.value("pos",QPoint(200,200)).toPoint();
    QSize size = settings.value("size",QSize(200,200)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::closeEvent(QCloseEvent *event){
    this->ui->mdiArea->closeAllSubWindows();
    if(ui->mdiArea->activeSubWindow()){
        event->ignore();
    }else{
        writeSetting();
        event->accept();
    }
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


void MainWindow::on_actionSave_triggered()
{
    if(activeMdiChild() && activeMdiChild()->save())
        ui->statusBar->showMessage(tr("保存成功"),2000);
}

void MainWindow::on_actionSaveAs_triggered()
{
    if(activeMdiChild() && activeMdiChild()->saveAs())
        ui->statusBar->showMessage(tr("保存成功"),2000);
}

void MainWindow::on_actionUndo_triggered()
{
    if(activeMdiChild()){
        activeMdiChild()->undo();
    }
}

void MainWindow::on_actionRedo_triggered()
{
    if(activeMdiChild()){
        activeMdiChild()->redo();
    }
}

void MainWindow::on_actionClose_triggered()
{
    ui->mdiArea->closeActiveSubWindow();
}

void MainWindow::on_actionCloseAll_triggered()
{
    ui->mdiArea->closeAllSubWindows();
}

void MainWindow::on_actionCut_triggered()
{
    if(activeMdiChild()){
        this->board->setText(activeMdiChild()->textCursor().selectedText());
        activeMdiChild()->textCursor().deleteChar();
    }
}

void MainWindow::on_actionCopy_triggered()
{
    if(activeMdiChild()){
        this->board->setText(activeMdiChild()->textCursor().selectedText());
    }
}

void MainWindow::on_actionPaste_triggered()
{
    if(activeMdiChild()){
        activeMdiChild()->textCursor().insertText(this->board->text());
    }
}

void MainWindow::on_actionNext_triggered()
{
//    QList<QMdiSubWindow*> windows = ui->mdiArea->subWindowList();
//    for(int i = 0; i < windows.size(); i++ ){
//        MdiChild *child = qobject_cast<MdiChild*>(windows.at(i)->widget());
//        if(child == activeMdiChild()){
//            setActiveSubWindow(windows.at((i+1)%windows.size()));
//            return;
//        }
//    }
    ui->mdiArea->activateNextSubWindow();
}

void MainWindow::on_actionPrevious_triggered()
{
//    QList<QMdiSubWindow*> windows = ui->mdiArea->subWindowList();
//    for(int i = 0; i < windows.size(); i++ ){
//        MdiChild *child = qobject_cast<MdiChild*>(windows.at(i)->widget());
//        if(child == activeMdiChild()){
//            setActiveSubWindow(windows.at((i-1+windows.size())%windows.size()));
//            return;
//        }
//    }
    ui->mdiArea->activatePreviousSubWindow();
}

void MainWindow::on_actionTile_triggered()
{
    ui->mdiArea->tileSubWindows();
}

void MainWindow::on_actionCascade_triggered()
{
    ui->mdiArea->cascadeSubWindows();
}

void MainWindow::on_actionExit_triggered()
{
    qApp->closeAllWindows();
}
