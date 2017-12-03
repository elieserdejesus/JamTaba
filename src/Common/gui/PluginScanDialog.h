#ifndef PLUGINSCANDIALOG_H
#define PLUGINSCANDIALOG_H

#include <QDialog>

namespace Ui {
class PluginScanDialog;
}

class PluginScanDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PluginScanDialog(QWidget *parent = 0);
    ~PluginScanDialog();
    void setCurrentScaning(const QString &pluginPath);
    void cleanFoundPluginsList();
    void addFoundedPlugin(const QString &pluginName);

private:
    Ui::PluginScanDialog *ui;
};

#endif // PLUGINSCANDIALOG_H
