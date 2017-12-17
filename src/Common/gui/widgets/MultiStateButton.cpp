#include "MultiStateButton.h"
#include <QtGlobal>
#include <QStyle>

MultiStateButton::MultiStateButton(QWidget *parent, QList<QImage> icons) :
    QPushButton(parent),
    states(icons.size()),
    currentState(0),
    icons(icons)
{
    setState(0);
}

void MultiStateButton::setState(quint8 newState)
{
    if (newState < states) {
        currentState = newState;
        if (texts.contains(currentState)) {
            QPushButton::setText(texts[currentState]);
        }

        setChecked(currentState > 0);

        style()->unpolish(this);
        style()->polish(this);

        updateIcon();

        repaint();

        emit stateChanged();
    }
}

void MultiStateButton::updateIcon()
{
    if (!icons.empty() && currentState < icons.size())
        setIcon(QIcon(QPixmap::fromImage(icons.at(currentState))));
}

void MultiStateButton::setIcons(QList<QImage> icons)
{
    this->icons = icons;

    updateIcon();
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
