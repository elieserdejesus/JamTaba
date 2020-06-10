#ifndef _INSTRUMENTS_MENU_
#define _INSTRUMENTS_MENU_

#include <QToolButton>

enum class InstrumentIndex : qint8
{
    // the order of enums are the same of the instrument icons in resources file
    AcousticGuitar,
    Banjo,
    ElectricBass,
    Video,
    Computer,
    DoubleBass,
    Drums,
    Guitar,
    Harmonica,
    JamTabaIcon,
    Keys,
    Mandolin,
    Mic,
    Pads,
    Percussion,
    Piano,
    TrollFace,
    Trumpet,
    Violin,
    Saxophone,
    Flute,
    Trombone
};


QString instrumentIndexToString(InstrumentIndex index);

InstrumentIndex stringToInstrumentIndex(const QString &string);


class InstrumentsButton : public QToolButton
{
    Q_OBJECT

public:
    InstrumentsButton(const QIcon &defaultIcon, const QList<QIcon> &icons, QWidget *parent = nullptr);
    void setInstrumentIcon(quint8 instrumentIcon);
    int getSelectedIcon() const;
signals:
    void iconChanged(quint8 iconIndex);

private slots:
    void showInstrumentIcons();

private:
    QWidget *toolBar;
    QList<QIcon> icons;
    int selectedIcon;

    QWidget *createToolBar(const QList<QIcon> &icons, const QSize &iconSize);

};

inline int InstrumentsButton::getSelectedIcon() const
{
    return selectedIcon;
}

#endif
