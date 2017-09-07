#include "BlinkableButton.h"

#include <QVariant>
#include <QStyle>
#include <QDateTime>

QList<BlinkableButton *> BlinkableButton::instances; // static instances list
quint64 BlinkableButton::lastBlinkToggle = 0;
quint32 BlinkableButton::blinkTime = 250;
bool BlinkableButton::blinkPropertySetted = false;

BlinkableButton::BlinkableButton(const QString &text, QWidget *parent) :
    QPushButton(text, parent),
    blinking(false)
{
    instances.append(this);
}

BlinkableButton::BlinkableButton(QWidget *parent) :
    QPushButton(parent),
    blinking(false)
{
    instances.append(this);
}

BlinkableButton::~BlinkableButton()
{
    instances.removeOne(this);
}

void BlinkableButton::updateAllBlinkableButtons()
{
    if (BlinkableButton::instances.isEmpty())
        return;

    const quint64 now = QDateTime::currentMSecsSinceEpoch();
    const bool canToggle = now - BlinkableButton::lastBlinkToggle >= BlinkableButton::blinkTime;
    if (canToggle) {
        blinkPropertySetted = !blinkPropertySetted;
        BlinkableButton::lastBlinkToggle = QDateTime::currentMSecsSinceEpoch();
        for (BlinkableButton *button : BlinkableButton::instances) {
            if (button->isBlinking()) {
                button->setBlinkProperty(BlinkableButton::blinkPropertySetted);
            }
        }
    }
}

void BlinkableButton::setBlinkTime(quint32 blinkTime)
{
    BlinkableButton::blinkTime = blinkTime;
}

void BlinkableButton::setBlinkProperty(bool propertySetted)
{
    setProperty("blinking", QVariant::fromValue(propertySetted));

    style()->unpolish(this);
    style()->polish(this);
}

void BlinkableButton::startBlink()
{
    blinking = true;
    setBlinkProperty(blinkPropertySetted);
}

void BlinkableButton::stopBlink()
{
    blinking = false;
    setBlinkProperty(false);
}
