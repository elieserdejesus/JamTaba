#include "InactivityDetector.h"

#include "NinjamController.h"
#include "MainWindow.h"
#include "MainController.h"

#include <QToolTip>

using namespace controller;

InactivityDetector::InactivityDetector(QWidget *parent, BlinkableButton *button, uint intervalsBeforeWarning) :
    QObject(parent),
    intervalsWithoutTransmit(0),
    intervalsBlinking(0),
    button(button),
    intervalsBeforeWarning(intervalsBeforeWarning),
    controller(nullptr)
{
    Q_ASSERT(button);

    connect(button, &BlinkableButton::toggled, [=](bool checked){
        if (!checked)
            reset();
    });
}

void InactivityDetector::initialize(NinjamController *controller)
{

    Q_ASSERT(controller);

    if (this->controller == controller)
        return;

    this->controller = controller;

    connect(controller, &NinjamController::startingNewInterval, this, &InactivityDetector::processIntervalBegin);

    connect(controller, &NinjamController::preparedToTransmit, this, &InactivityDetector::reset);

}

void InactivityDetector::processIntervalBegin()
{
    if (!button->isChecked()) {

        intervalsWithoutTransmit++;

        if (intervalsWithoutTransmit == intervalsBeforeWarning) {

            if (!button->isBlinking()) {

                QString message(tr("You are not transmiting for a long time! This is intentional?"));
                QToolTip::showText(button->mapToGlobal(button->rect().topRight()), message);

                button->startBlink();
            }
        }
        else {
            if (button->isBlinking()) {
                intervalsBlinking++;
                if (intervalsBlinking >= intervalsBeforeWarning)
                    reset(); // stop warning
            }
        }
    }
    else {
        reset();
    }
}

void InactivityDetector::deinitialize()
{
    reset();

    disconnect(controller, &NinjamController::startingNewInterval, this, &InactivityDetector::processIntervalBegin);
    disconnect(controller, &NinjamController::preparedToTransmit, this, &InactivityDetector::reset);

    controller = nullptr;
}

void InactivityDetector::reset()
{
    intervalsWithoutTransmit = 0;
    intervalsBlinking = 0;

    button->stopBlink();

    QToolTip::hideText();
}
