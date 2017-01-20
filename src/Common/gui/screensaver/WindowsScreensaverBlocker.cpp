#include "ScreensaverBlocker.h"
#include "Windows.h"

#include <QDateTime>
#include <QDebug>

#define REFRESH_PERIOD 30000 // in miliseconds

ScreensaverBlocker::ScreensaverBlocker()
{

}

ScreensaverBlocker::~ScreensaverBlocker()
{

}

void ScreensaverBlocker::update()
{
    static quint64 lastThreadStateUpdate = QDateTime::currentMSecsSinceEpoch();

    quint64 now = QDateTime::currentMSecsSinceEpoch();
    if (now - lastThreadStateUpdate >= REFRESH_PERIOD) {
        lastThreadStateUpdate = now;
        if(!SetThreadExecutionState(ES_DISPLAY_REQUIRED)) {
            qCritical() << "Can't disable screen saver!";
        }
    }
}
