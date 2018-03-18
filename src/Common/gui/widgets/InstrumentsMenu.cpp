#include "InstrumentsMenu.h"

#include <QMenu>
#include <QToolBar>
#include <QHBoxLayout>

InstrumentsButton::InstrumentsButton(const QIcon &defaultIcon, const QList<QIcon> &icons, QWidget *parent) :
    QToolButton(parent)
{
    setPopupMode(QToolButton::InstantPopup);
    setObjectName("instrumentButton");

    const QSize iconSize(32, 32);

    setIconSize(iconSize);
    setIcon(defaultIcon);

    toolBar = createToolBar(icons, iconSize);

    connect(this, &InstrumentsButton::clicked, this, &InstrumentsButton::showInstrumentIcons);
}

QWidget *InstrumentsButton::createToolBar(const QList<QIcon> &icons, const QSize &iconSize)
{
    auto toolBar = new QWidget(this);
    toolBar->setWindowFlags(Qt::Popup);
    toolBar->setObjectName("instrumentsMenu");

    auto vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(QMargins(3, 3, 3, 3));
    vLayout->setSpacing(3);

    toolBar->setLayout(vLayout);

    auto rows = 4;
    auto i = 0;
    for (int r = 0; r < rows; ++r) {
        auto widget = new QWidget(toolBar);
        auto hLayout = new QHBoxLayout();
        hLayout->setContentsMargins(QMargins(0, 0, 0, 0));
        widget->setLayout(hLayout);
        toolBar->layout()->addWidget(widget);

        for (auto c = 0; c < icons.size()/rows; ++c) {
            auto button = new QToolButton();
            button->setIconSize(iconSize);
            button->setIcon(icons.at(i));
            connect(button, &QToolButton::clicked, [=](){
                toolBar->close();
                setIcon(icons.at(i));
                emit iconSelected(i);
            });

            widget->layout()->addWidget(button);

            i++;
        }
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
