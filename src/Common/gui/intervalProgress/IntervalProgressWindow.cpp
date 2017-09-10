#include "IntervalProgressWindow.h"
#include "log/Logging.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtGlobal>
#include <QDialog>
#include <QIcon>

IntervalProgressWindow::IntervalProgressWindow(QWidget *parent, IntervalProgressDisplay::PaintShape paintShape, int beatsPerInterval, QList<int> accentBeats, bool showingAccents)
    :QDialog(parent)
{

    setWindowIcon(QIcon(":/images/logo.png"));

    progressDisplay = new IntervalProgressDisplay(this);
    progressDisplay->setObjectName(QStringLiteral("intervalPanel"));
    progressDisplay->setBeatsPerInterval(beatsPerInterval);
    progressDisplay->setAccentBeats(accentBeats);
    progressDisplay->setShowAccents(showingAccents);
    setIntervalProgressShape(paintShape);

    progressDisplay->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

    setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(progressDisplay, 1);

    setLayout(mainLayout);

    setSizeGripEnabled(true);
}

void IntervalProgressWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    emit windowClosed();
}

void IntervalProgressWindow::setIntervalProgressShape(IntervalProgressDisplay::PaintShape paintShape)
{
    progressDisplay->setPaintMode(paintShape);
    if (paintShape == IntervalProgressDisplay::LINEAR) {
        progressDisplay->setMinimumHeight(150);
        progressDisplay->setMinimumWidth(780);
    }
    else{
        int minWidth = paintShape == IntervalProgressDisplay::ELLIPTICAL ? 500 : 300;
        progressDisplay->setMinimumHeight(300);
        progressDisplay->setMinimumWidth(minWidth);
    }
}

void IntervalProgressWindow::setCurrentBeat(int currentBeat)
{
    progressDisplay->setCurrentBeat(currentBeat);
}

 void IntervalProgressWindow::setBeatsPerInterval(int beatsPerInterval)
 {
     progressDisplay->setBeatsPerInterval(beatsPerInterval);
 }

 void IntervalProgressWindow::setAccentBeats(QList<int> accentBeats)
 {
     qCDebug(jtNinjamGUI) << "IntervalProgressDisplay::setAccentBeats" << accentBeats;

     progressDisplay->setAccentBeats(accentBeats);
 }

//QSize IntervalProgressWindow::minimumSizeHint() const
//{
//    QSize size = QWidget::minimumSizeHint();
//    IntervalProgressDisplay::PaintMode paintMode = progressDisplay->getPaintMode();
//    switch (paintMode) {
//    case IntervalProgressDisplay::CIRCULAR:
//    case IntervalProgressDisplay::PIE:
//        size.setHeight(400);
//        size.setWidth(size.height());
//        break;
//    case IntervalProgressDisplay::ELLIPTICAL:
//        size.setHeight(400);
//        size.setWidth(size.height() * 2);
//        break;
//    case IntervalProgressDisplay::LINEAR:
//        size.setHeight(250);
//        size.setWidth(height() * 5);
//        break;
//    }
//    return size;
//}

//QSize IntervalProgressWindow::sizeHint() const
//{
//    return minimumSizeHint();
//}

void IntervalProgressWindow::setShowAccents(bool showAccents)
{
    qCDebug(jtNinjamGUI) << "IntervalProgressDisplay::setShowAccents" << showAccents;

    progressDisplay->setShowAccents(showAccents);
}
