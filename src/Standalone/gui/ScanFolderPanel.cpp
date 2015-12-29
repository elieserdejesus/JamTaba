#include "ScanFolderPanel.h"

#include <QHBoxLayout>
#include <QPushButton>

ScanFolderPanel::ScanFolderPanel(QString folder) :
    QWidget(nullptr),
    removeButton(nullptr),
    scanFolder(folder)
{
    removeButton = new QPushButton(QIcon(":/images/less.png"), "");
    removeButton->setToolTip("Remove this folder from scanning");

    QHBoxLayout *layout = new QHBoxLayout();
    this->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(removeButton);
    layout->addWidget(new QLabel(scanFolder, this), 1.0);
}
