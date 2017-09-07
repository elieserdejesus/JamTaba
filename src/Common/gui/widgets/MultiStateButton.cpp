#include "MultiStateButton.h"
#include <QtGlobal>
#include <QStyle>

MultiStateButton::MultiStateButton(quint8 states, QWidget *parent) :
    QPushButton(parent),
    states(qMax(states, (quint8)2)),
    currentState(0)
{

}

void MultiStateButton::setState(quint8 newState)
{
    if (newState < states && newState != currentState) {
        currentState = newState;
        if (texts.contains(currentState)) {
            QPushButton::setText(texts[currentState]);
        }

        style()->unpolish(this);
        style()->polish(this);

        setChecked(currentState > 0);
        repaint();

        emit stateChanged();
    }
}

void MultiStateButton::nextCheckState()
{
    setState((currentState + 1) % states);
}

void MultiStateButton::setText(const QString &text, quint8 state)
{
    texts.insert(state, text);

    if (state == currentState)
        QPushButton::setText(text);

}
