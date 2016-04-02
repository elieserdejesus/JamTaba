#include "TestMainWindow.h"
#include "gui/intervalProgress/IntervalProgressDisplay.h"
#include <QPushButton>
#include <QDebug>

    TestMainWindow::TestMainWindow()
        :QFrame(),
        currentBeat(0)
    {
        refreshTimerID = startTimer(400);

        setMinimumSize(600, 150);

        progressDisplay = new IntervalProgressDisplay(this);
        progressDisplay->setBeatsPerAccent(4);
        progressDisplay->setShowAccents(false);

        QVBoxLayout *mainLayout = new QVBoxLayout();
        mainLayout->addWidget(progressDisplay, 1);
        this->setLayout(mainLayout);

        QComboBox *combo = new QComboBox();
        combo->addItem("Line");
        combo->addItem("Circle");
        combo->addItem("Ellipse");
        combo->addItem("Pie");

        QPushButton *accentsButton = new QPushButton("Accents");
        accentsButton->setCheckable(true);

        QHBoxLayout *controlsLayout = new QHBoxLayout();
        controlsLayout->addWidget(combo);
        controlsLayout->addWidget(accentsButton);

        mainLayout->addLayout(controlsLayout);

        connect(combo, SIGNAL(currentIndexChanged(QString)), this, SLOT(setShape(QString)));
        connect(accentsButton, SIGNAL(toggled(bool)), this, SLOT(toggleAccents(bool)));

        progressDisplay->setBeatsPerInterval(64);
        currentBeat = 0;//progressDisplay->getBeatsPerInterval()/2 - 1;
        //progressDisplay->setCurrentBeat(currentBeat);
        combo->setCurrentIndex(0);
    }

    TestMainWindow::~TestMainWindow()
    {
        killTimer(refreshTimerID);
    }

    void TestMainWindow::timerEvent(QTimerEvent *){
        int beatsPerInterval = progressDisplay->getBeatsPerInterval();
        currentBeat = (currentBeat + 1) % beatsPerInterval;
        progressDisplay->setCurrentBeat(currentBeat);
        update();
    }

    void TestMainWindow::toggleAccents(bool showAccents)
    {
        progressDisplay->setShowAccents(showAccents);
    }

    void TestMainWindow::setShape(QString shape)
    {
        if ("Circle" == shape){
            progressDisplay->setPaintMode(IntervalProgressDisplay::CIRCULAR);
        }
        else if ("Line" == shape) {
            progressDisplay->setPaintMode(IntervalProgressDisplay::LINEAR);
        }
        else if("Pie" == shape){
            progressDisplay->setPaintMode(IntervalProgressDisplay::PIE);
        }
        else{
            progressDisplay->setPaintMode(IntervalProgressDisplay::ELLIPTICAL);
        }
        update();
    }


