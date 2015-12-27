#include "AutoUpdaterDialog.h"
#include "ui_AutoUpdaterDialog.h"

AutoUpdaterDialog::AutoUpdaterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoUpdaterDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);

}

AutoUpdaterDialog::~AutoUpdaterDialog()
{
    delete ui;
}

void AutoUpdaterDialog::setDownloadProgress(int progressValue){
    ui->progressBar->setValue(progressValue);
}

void AutoUpdaterDialog::setStatusText(QString text){
    ui->label->setText(text);
}
