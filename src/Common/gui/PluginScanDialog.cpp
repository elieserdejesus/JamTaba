#include "PluginScanDialog.h"
#include "ui_PluginScanDialog.h"

PluginScanDialog::PluginScanDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PluginScanDialog)
{
    ui->setupUi(this);
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

PluginScanDialog::~PluginScanDialog()
{
    delete ui;
}

void PluginScanDialog::cleanFoundPluginsList()
{
    ui->plainTextEdit->clear();
}

void PluginScanDialog::addFoundedPlugin(const QString &pluginName)
{
    ui->plainTextEdit->appendPlainText(pluginName);
}

void PluginScanDialog::setCurrentScaning(const QString &pluginPath)
{
    ui->labelPluginText->setText(tr("scanning %1").arg(pluginPath));
    update();
}
