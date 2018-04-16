#include "InstrumentsMenu.h"

#include <QMenu>
#include <QToolBar>
#include <QHBoxLayout>

InstrumentIndex stringToInstrumentIndex(const QString &string)
{
    auto str = string.toLower().trimmed();
    if (str.contains("acoustic guitar"))
        return InstrumentIndex::AcousticGuitar;

    if (str.contains("guitar"))
        return InstrumentIndex::Guitar;

    if (str.contains("key"))
        return InstrumentIndex::Keys;

    if (str.contains("piano"))
        return InstrumentIndex::Piano;

    if (str.contains("voice") || str.contains("sing"))
        return InstrumentIndex::Mic;

    if (str.contains("drum"))
        return InstrumentIndex::DrumStick;

    if (str.contains("mandolin"))
        return InstrumentIndex::Mandolin;

    if (str.contains("violin"))
        return InstrumentIndex::Violin;

    if (str.contains("double"))
        return InstrumentIndex::DoubleBass;

    if (str.contains("double bass"))
        return InstrumentIndex::DoubleBass;

    if (str.contains("bass"))
        return InstrumentIndex::ElectricBass;

    if (str.contains("trumpet"))
        return InstrumentIndex::Trumpet;

    if (str.contains("banjo"))
        return InstrumentIndex::Banjo;

    if (str.contains("loops"))
        return InstrumentIndex::Gramophone;

    if (str.contains("Percussion"))
        return InstrumentIndex::Percussion;

    if (str.contains("TrollFace"))
        return InstrumentIndex::TrollFace;

    return InstrumentIndex::JamTabaIcon;
}

QString instrumentIndexToString(InstrumentIndex index)
{
    switch (index) {
    case InstrumentIndex::AcousticGuitar:   return "Acoustic Guitar";
    case InstrumentIndex::Banjo:            return "Banjo";
    case InstrumentIndex::ElectricBass:     return "Bass";
    case InstrumentIndex::DoubleBass:       return "Double Bass";
    case InstrumentIndex::Drums:            // drums
    case InstrumentIndex::DrumStick:        return "Drums";
    case InstrumentIndex::Gramophone:       return "Loops";
    case InstrumentIndex::Guitar:           return "Guitar";
    case InstrumentIndex::JamTabaIcon:      return "";
    case InstrumentIndex::Keys:             return "Keys";
    case InstrumentIndex::Piano:            return "Piano";
    case InstrumentIndex::Mandolin:         return "Mandolin";
    case InstrumentIndex::Mic:              return "Voice";
    case InstrumentIndex::Percussion:       return "Percussion";
    case InstrumentIndex::TrollFace:        return "TrollFace";
    case InstrumentIndex::Trumpet:          return "Trumpet";
    case InstrumentIndex::Violin:           return "Violin";
    }

    return "";
}

InstrumentsButton::InstrumentsButton(const QIcon &defaultIcon, const QList<QIcon> &icons, QWidget *parent) :
    QToolButton(parent),
    icons(icons),
    selectedIcon(-1)
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
    if (instrumentIcon < icons.size()) {
        setIcon(icons.at(instrumentIcon));
        selectedIcon = instrumentIcon;
    }
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
            selectedIcon = i;
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
