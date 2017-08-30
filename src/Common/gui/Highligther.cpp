#include "Highligther.h"
#include <QWidget>
#include <QStyle>
#include <QVariant>

Highligther *Highligther::instance = nullptr;

Highligther::Highligther() :
    lastHighlightedWidget(nullptr)
{
    QObject::connect(&timer, &QTimer::timeout, this, &Highligther::on_timerStopped);
}

void Highligther::highlight(QWidget *w)
{
    QObject::connect(w, &QWidget::destroyed, this, &Highligther::on_objectDestroyed);

    if (lastHighlightedWidget) {
        timer.blockSignals(true);
        timer.stop();
        setWidgetHighlightStatus(lastHighlightedWidget, false);
        timer.blockSignals(false);
    }
    setWidgetHighlightStatus(w, true);
    timer.start(HIGHLIGHT_TIME);
    lastHighlightedWidget = w;
}

void Highligther::stopHighlight()
{
    if (lastHighlightedWidget) {
        setWidgetHighlightStatus(lastHighlightedWidget, false);
        QObject::disconnect(lastHighlightedWidget, &QWidget::destroyed, this, &Highligther::on_objectDestroyed);
        lastHighlightedWidget = nullptr;
    }
}

void Highligther::on_timerStopped()
{
    stopHighlight();
}

void Highligther::on_objectDestroyed(QObject *ob)
{
    if (ob && ob == lastHighlightedWidget)
        lastHighlightedWidget = nullptr;
}

void Highligther::setWidgetHighlightStatus(QWidget *w, bool highlighted)
{
    w->setProperty("highlighted", QVariant(highlighted));
    w->style()->unpolish(w);
    w->style()->polish(w);
    w->update();
}

Highligther *Highligther::getInstance()
{
    if (!instance)
        instance = new Highligther();

    return instance;
}

Highligther::~Highligther()
{
    QObject::disconnect(&timer, SIGNAL(timeout()), this, SLOT(on_timerStopped()));
}
