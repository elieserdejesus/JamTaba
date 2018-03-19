#include "InstrumentsMenu.h"

#include <QMenu>
#include <QToolBar>
#include <QHBoxLayout>

InstrumentsButton::InstrumentsButton(const QIcon &defaultIcon, const QList<QIcon> &icons, QWidget *parent) :
    QToolButton(parent),
    icons(icons)
{
    setPopupMode(QToolButton::InstantPopup);
    setObjectName("instrumentsButton");

    const QSize iconSize(32, 32);

    setIconSize(iconSize);
    setIcon(defaultIcon);

    toolBar = createToolBar(icons, iconSize);

    connect(this, &InstrumentsButton::clicked, this, &InstrumentsButton::showInstrumentIcons);
}

void InstrumentsButton::setInstrumentIcon(quint8 instrumentIcon)
{
    if (instrumentIcon < icons.size())
        setIcon(icons.at(instrumentIcon));
}

QWidget *InstrumentsButton::createToolBar(const QList<QIcon> &icons, const QSize &iconSize)
{
    auto toolBar = new QWidget(this);
    toolBar->setWindowFlags(Qt::Popup);
    toolBar->setObjectName("instrumentsMenu");

    auto gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(QMargins(3, 3, 3, 3));
    gridLayout->setSpacing(3);

    toolBar->setLayout(gridLayout);

    const auto columns = 6;
    for (int i = 0; i < icons.size(); ++i) {
        auto button = new QToolButton();
        button->setIconSize(iconSize);
        button->setIcon(icons.at(i));

        connect(button, &QToolButton::clicked, [=](){
            toolBar->close();
            setIcon(icons.at(i));
            emit iconSelected(i);
        });

        auto rowIndex = i / columns;
        auto colIndex = i % columns;
        gridLayout->addWidget(button, rowIndex, colIndex);
    }

    return toolBar;
}

void InstrumentsButton::showInstrumentIcons()
{
    if (toolBar) {
        toolBar->move(mapTo(this, QCursor::pos()));
        toolBar->show();
    }
}
