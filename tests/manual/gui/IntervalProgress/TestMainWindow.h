#ifndef TESTMAINWINDOW_H
#define TESTMAINWINDOW_H

#include <QFrame>
#include <QTimerEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include "gui/intervalProgress/IntervalProgressDisplay.h"

class TestMainWindow : public QFrame
{
    Q_OBJECT

public:
    TestMainWindow();
    ~TestMainWindow();

protected:
    void timerEvent(QTimerEvent *);
private:
    int refreshTimerID;
    IntervalProgressDisplay *progressDisplay;
    int currentBeat;
private slots:
    void setShape(QString shape);
    void toggleAccents(bool showAccents);
};


#endif // TESTMAINWINDOW_H
