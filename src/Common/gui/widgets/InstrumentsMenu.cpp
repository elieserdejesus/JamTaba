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

    if (str.contains("voice") || str.contains("sing") || str.contains("mic"))
        return InstrumentIndex::Mic;

    if (str.contains("drum"))
        return InstrumentIndex::Drums;

    if (str.contains("mandolin"))
        return InstrumentIndex::Mandolin;

    if (str.contains("violin"))
        return InstrumentIndex::Violin;

    if (str.contains("double"))
        return InstrumentIndex::DoubleBass;

    if (str.contains("bass"))
        return InstrumentIndex::ElectricBass;

    if (str.contains("trumpet"))
        return InstrumentIndex::Trumpet;

    if (str.contains("banjo"))
        return InstrumentIndex::Banjo;

    if (str.contains("pads"))
        return InstrumentIndex::Pads;

    if (str.contains("computer"))
        return InstrumentIndex::Computer;

    if (str.contains("percussion") || str.contains("conga"))
        return InstrumentIndex::Percussion;

    if (str.contains("troll"))
        return InstrumentIndex::TrollFace;

    if (str.contains("video"))
        return InstrumentIndex::Video;

    if (str.contains("harmonica") || str.contains("blues harp"))
        return InstrumentIndex::Harmonica;

    if (str.contains("sax"))
        return InstrumentIndex::Saxophone;

    if (str.contains("flute"))
        return InstrumentIndex::Flute;

    if (str.contains("trombone"))
        return InstrumentIndex::Trombone;

    return InstrumentIndex::JamTabaIcon;
}

QString instrumentIndexToString(InstrumentIndex index)
{
    switch (index) {
    case InstrumentIndex::AcousticGuitar:   return "Acoustic Guitar";
    case InstrumentIndex::Banjo:            return "Banjo";
    case InstrumentIndex::ElectricBass:     return "Bass";
    case InstrumentIndex::DoubleBass:       return "Double Bass";
    case InstrumentIndex::Drums:            return "Drums";
    case InstrumentIndex::Computer:         return "Computer";
    case InstrumentIndex::Pads:             return "Pads";
    case InstrumentIndex::Guitar:           return "Guitar";
    case InstrumentIndex::JamTabaIcon:      return "";
    case InstrumentIndex::Keys:             return "Keys";
    case InstrumentIndex::Piano:            return "Piano";
    case InstrumentIndex::Mandolin:         return "Mandolin";
    case InstrumentIndex::Mic:              return "Voice";
    case InstrumentIndex::Percussion:       return "Percussion";
    case InstrumentIndex::TrollFace:        return "Troll";
    case InstrumentIndex::Trumpet:          return "Trumpet";
    case InstrumentIndex::Violin:           return "Violin";
    case InstrumentIndex::Video:            return "Video";
    case InstrumentIndex::Harmonica:        return "Harmonica";
    case InstrumentIndex::Saxophone:        return "Saxophone";
    case InstrumentIndex::Flute:            return "Flute";
    case InstrumentIndex::Trombone:         return "Trombone";
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

void InstrumentsButton::setInstrumentIcon(quint8 instrumentIconIndex)
{
    if (instrumentIconIndex < icons.size()) {
        setIcon(icons.at(instrumentIconIndex));
        selectedIcon = instrumentIconIndex;
        emit iconChanged(selectedIcon);
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

    const auto columns = 5;
    for (int i = 0; i < icons.size(); ++i) {
        auto button = new QToolButton();
        button->setIconSize(iconSize);
        button->setIcon(icons.at(i));

        connect(button, &QToolButton::clicked, [=](){
            toolBar->close();
            setInstrumentIcon(i);
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
