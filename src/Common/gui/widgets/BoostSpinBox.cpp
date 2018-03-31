#include "BoostSpinBox.h"

#include <QToolTip>
#include <QBoxLayout>
#include <QStyle>

BoostSpinBox::BoostSpinBox(QWidget *parent) :
    QFrame(parent),
    boostValue(0),
    buttonDecrease(new QToolButton()),
    buttonIncrease(new QToolButton())
{
    setObjectName("boostSpinBox");

    auto vLayout = new QBoxLayout(QHBoxLayout::TopToBottom);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    setLayout(vLayout);

    vLayout->addWidget(buttonIncrease, 0, Qt::AlignCenter);
    vLayout->addWidget(buttonDecrease, 0, Qt::AlignCenter);

    buttonIncrease->setToolButtonStyle(Qt::ToolButtonIconOnly);
    buttonDecrease->setToolButtonStyle(Qt::ToolButtonIconOnly);

    connect(buttonIncrease, &QToolButton::clicked, this, &BoostSpinBox::increaseBoost);
    connect(buttonDecrease, &QToolButton::clicked, this, &BoostSpinBox::decreaseBoost);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    setOrientation(Qt::Vertical);
}

void BoostSpinBox::updateStyleSheet()
{
    style()->unpolish(this);
    style()->polish(this);

    style()->unpolish(buttonDecrease);
    style()->polish(buttonDecrease);

    style()->unpolish(buttonIncrease);
    style()->polish(buttonIncrease);
}

void BoostSpinBox::setOrientation(Qt::Orientation orientation)
{
    QBoxLayout *boxLayout = static_cast<QHBoxLayout *>(layout());
    if (boxLayout) {
        if (orientation == Qt::Vertical) {
            boxLayout->setDirection(QBoxLayout::TopToBottom);
            buttonIncrease->setArrowType(Qt::UpArrow);
            buttonDecrease->setArrowType(Qt::DownArrow);
        }
        else {
            boxLayout->setDirection(QBoxLayout::RightToLeft);
            buttonIncrease->setArrowType(Qt::RightArrow);
            buttonDecrease->setArrowType(Qt::LeftArrow);
        }
    }
}

void BoostSpinBox::setToMax()
{
    if (boostValue == MAX_BOOST_VALUE)
        return;

    boostValue = MAX_BOOST_VALUE;

    updateButtons();
    updateToolTip();

    emit boostChanged(boostValue);
}

void BoostSpinBox::setToOff()
{
    if (boostValue == 0)
        return;

    boostValue = 0;

    updateButtons();
    updateToolTip();

    emit boostChanged(boostValue);
}

void BoostSpinBox::setToMin()
{
    if (boostValue == MIN_BOOST_VALUE)
        return;

    boostValue = MIN_BOOST_VALUE;

    updateButtons();
    updateToolTip();

    emit boostChanged(boostValue);
}

void BoostSpinBox::updateToolTip()
{
    QString boostText = " OFF [0 dB]";
    if (boostValue != 0) {
        boostText = QString(" (%1%2 dB)")
                .arg(boostValue < 0 ? QString("-") : QString("+"))
                .arg(qAbs(boostValue));
    }

    QString toolTipText = tr("Boost") + boostText;

    setToolTip(toolTipText);
    buttonDecrease->setToolTip(toolTipText);
    buttonIncrease->setToolTip(toolTipText);
}

void BoostSpinBox::updateButtons()
{
    buttonDecrease->setEnabled(canDecrease());
    buttonIncrease->setEnabled(canIncrease());
}

void BoostSpinBox::increaseBoost()
{
    if (!canIncrease())
        return;

    boostValue += BOOST_STEP;

    updateButtons();
    updateToolTip();

    emit boostChanged(boostValue);

    showToolTip();
}

void BoostSpinBox::showToolTip()
{
    QToolTip::showText(mapToGlobal(QPoint(20,0)), toolTip());
}

void BoostSpinBox::decreaseBoost()
{
    if (!canDecrease())
        return;

    boostValue -= BOOST_STEP;

    updateButtons();
    updateToolTip();

    emit boostChanged(boostValue);

    showToolTip();
}
