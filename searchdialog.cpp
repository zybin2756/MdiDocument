#include "searchdialog.h"
#include "ui_searchdialog.h"

searchDialog::searchDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::searchDialog)
{
    ui->setupUi(this);
}

searchDialog::~searchDialog()
{
    delete ui;
}
