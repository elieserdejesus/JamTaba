#include "MultiStateButton.h"
#include <QtGlobal>
#include <QStyle>

MultiStateButton::MultiStateButton(quint8 states, QWidget *parent) :
    QPushButton(parent),
    states(qMax(states, (quint8)2)),
    currentState(0)
{

}

void MultiStateButton::nextCheckState()
{
    currentState = (currentState + 1) % states;

    style()->unpolish(this);
    style()->polish(this);

    setChecked(currentState > 0);
    repaint();
}
