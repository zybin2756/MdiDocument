#ifndef MDICHILD_H
#define MDICHILD_H

#include <QWidget>
#include <QTextEdit>

class MdiChild : public QTextEdit
{
    Q_OBJECT
public:
    explicit MdiChild(QWidget *parent = nullptr);
    void newFile();//新建文件
    bool loadFile(const QString &fileName);//打开文件
    bool saveFile(const QString &fileName); //保存文件
    bool saveAs(); //另存为
    bool save(); //保存
    QString currentFile(){
        return curFileName;
    }
protected:
//    void closeEvent(QCloseEvent * event);
private slots:
    void documentWasModified();

private:
    bool maybeSave();
    void setCurrentFile(const QString &file);
    QString curFileName;
    bool isUntitled;
};

#endif // MDICHILD_H
