#include "BlinkableButton.h"

#include <QVariant>
#include <QStyle>

BlinkableButton::BlinkableButton(const QString &text)
    : QPushButton(text)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateBlinkProperty()));
}

BlinkableButton::BlinkableButton(const QString &text, quint32 blinkTime)
    : QPushButton(text)
{
    timer.setInterval(blinkTime);
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateBlinkProperty()));
}

void BlinkableButton::setBlinkTime(quint32 blinkTime)
{
    timer.setInterval(blinkTime);
}

void BlinkableButton::updateBlinkProperty(bool propertySetted)
{
    setProperty("blinking", QVariant::fromValue(propertySetted));

    style()->unpolish(this);
    style()->polish(this);
}

void BlinkableButton::updateBlinkProperty()
{
    bool propertySetted = property("blinking").toBool();
    updateBlinkProperty(!propertySetted);
}

void BlinkableButton::startBlink()
{
    timer.start();
}

void BlinkableButton::stopBlink()
{
    timer.stop();

    updateBlinkProperty(false);
}
