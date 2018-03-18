#ifndef _INSTRUMENTS_MENU_
#define _INSTRUMENTS_MENU_

#include <QToolButton>

class InstrumentsButton : public QToolButton
{
    Q_OBJECT

public:
    InstrumentsButton(const QIcon &defaultIcon, const QList<QIcon> &icons, QWidget *parent = nullptr);

signals:
    void iconSelected(quint8 iconIndex);

private slots:
    void showInstrumentIcons();

private:
    QWidget *toolBar;

    QWidget *createToolBar(const QList<QIcon> &icons, const QSize &iconSize);
};

#endif
