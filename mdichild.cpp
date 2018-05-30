#include "mdichild.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QFileInfo>
#include <QApplication>
#include <QCloseEvent>
#include <QPushButton>

MdiChild::MdiChild(QWidget *parent) : QTextEdit(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    this->isUntitled = true;
}

void MdiChild::newFile()
{
    static int seqNum = 1;
    this->isUntitled = true;
    this->curFileName = tr("新建文档%1.txt").arg(seqNum++);
    setWindowTitle(this->curFileName+"[*]"+tr("- 多文档编辑器"));
    connect(document(),SIGNAL(contentsChanged()),this, SLOT(documentWasModified()));

}

void MdiChild::documentWasModified()
{
    setWindowModified(document()->isModified());
}

void MdiChild::setCurrentFile(const QString &fileName){
    this->curFileName = fileName;
}

bool MdiChild::loadFile(const QString &fileName){
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,tr("多文档编辑器"),
                             tr("无法读取文件 %1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);
    connect(document(),SIGNAL(contentsChanged()),this,SLOT(documentWasModified()));
    return true;
}
