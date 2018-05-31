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

void MdiChild::setCurrentFile(const QString &file){
    this->curFileName = QFileInfo(file).canonicalFilePath();
    this->isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(this->userFirendlyPath());
}

QString MdiChild::userFirendlyPath(){
    return QFileInfo(this->curFileName).fileName();
}


bool MdiChild::save(){
    if(isUntitled){
        return saveAs();
    }else{
        return saveFile(this->curFileName);
    }
}

bool MdiChild::saveFile(const QString &fileName){
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,tr("多文档编辑器"),tr("无法打开文档 %1 %2").arg(fileName).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << toPlainText();
    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);
    return true;
}

bool MdiChild::saveAs(){
    QString fileName = QFileDialog::getSaveFileName(this, tr("另存为"), this->curFileName);
    if(fileName.isEmpty()){
        return false;
    }

    return this->saveFile(fileName);
}

void MdiChild::closeEvent(QCloseEvent *event){
    if(maybeSave()){
        event->accept();
    }else{
        event->ignore();
    }
}

bool MdiChild::maybeSave(){

    if(document()->isModified()){
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("多文档编辑器"));
        msgBox.setText(tr("文档%1已修改，是否保存").arg(this->curFileName));
        msgBox.setIcon(QMessageBox::Warning);
        QPushButton *yesBtn = msgBox.addButton(tr("是(&Y)"),QMessageBox::YesRole);
        msgBox.addButton(tr("否(&N)"),QMessageBox::NoRole);
        QPushButton *cancelBtn = msgBox.addButton(tr("取消(&C)"),QMessageBox::RejectRole);
        msgBox.exec();
        if(msgBox.clickedButton() == yesBtn){
            this->save();
        }else if(msgBox.clickedButton() == cancelBtn){
            return false;
        }
        return true;
    }
    return true;
}


