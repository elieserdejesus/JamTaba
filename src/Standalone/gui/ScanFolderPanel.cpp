#include "ScanFolderPanel.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

ScanFolderPanel::ScanFolderPanel(QString folder) :
    QWidget(nullptr),
    removeButton(nullptr),
    scanFolder(folder)
{

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));

    removeButton = new QPushButton(QIcon(":/images/less.png"), "");
    removeButton->setToolTip("Remove this folder from scanning");
    removeButton->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed));

    QHBoxLayout *layout = new QHBoxLayout();
    this->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    layout->addWidget(removeButton);
    QLabel *label = new QLabel(scanFolder);
    label->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));
    layout->addWidget(label, 1);
}
