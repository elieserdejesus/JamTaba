#include "pluginwindow.h"
#include "plugins/guis.h"
#include <QVBoxLayout>

PluginWindow::PluginWindow(PluginGui *pluginGui, QWidget *parent)
    :QDialog(parent), pluginGui(pluginGui)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(pluginGui);
    setAttribute( Qt::WA_DeleteOnClose, true );
    setWindowTitle(pluginGui->getPluginName());

}

PluginWindow::~PluginWindow()
{
    pluginGui->setParent(nullptr);//avoid delete pluginGui instance
}

