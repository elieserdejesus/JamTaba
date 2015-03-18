#include "pluginscandialog.h"
#include "ui_pluginscandialog.h"

PluginScanDialog::PluginScanDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PluginScanDialog)
{
    ui->setupUi(this);
    setModal(true);
}

PluginScanDialog::~PluginScanDialog()
{
    delete ui;
}

void PluginScanDialog::setText(QString text){

    ui->labelPluginText->setText(text);
    update();
}
