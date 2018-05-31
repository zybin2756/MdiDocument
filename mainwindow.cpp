#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mdichild.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_triggered()
{
    MdiChild *child = new MdiChild;
    ui->mdiArea->addSubWindow(child);
    child->newFile();
    child->show();
}

void MainWindow::on_actionOpen_triggered()
{
    MdiChild *child = new MdiChild;
    ui->mdiArea->addSubWindow(child);
    QString fileName = QFileDialog::getOpenFileName(this,tr("请选择文件"),"e:","*txt");
    child->loadFile(fileName);
    child->show();
}
