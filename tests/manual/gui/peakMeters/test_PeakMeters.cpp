#include <QApplication>
#include <QFrame>
#include <QPaintEvent>
#include <QShowEvent>
#include <QDebug>
#include <QTimer>
#include <QTimerEvent>
#include <QTime>
#include <QHBoxLayout>
#include <QPushButton>
#include <cmath>
#include <gui/widgets/PeakMeter.h>

#include "Utils.h"

class TestMainWindow : public QFrame
{

public:
    TestMainWindow()
        :QFrame()
    {
        refreshTimerID = startTimer(1000/30);

        QHBoxLayout *layout = new QHBoxLayout();
        meter = new AudioMeter(this);
        meter->setMinimumWidth(10);

        layout->addWidget(meter);

        QVBoxLayout *buttonsLayout = new QVBoxLayout();
        float dbValues[] = {0.0, -6.0, -12.0, 3.0, 6.0};
        for (float db : dbValues) {
            QPushButton *button = new QPushButton(QString::number(db) + " dB");
            connect(button, &QPushButton::clicked, [=](){
                float db = button->text().replace(" dB", "").toFloat();
                meter->setPeak(Utils::dbToLinear(db), 0);

            });
            buttonsLayout->addWidget(button);
        }

        layout->addSpacing(20);
        layout->addLayout(buttonsLayout);

        setLayout(layout);

        setMinimumWidth(100);
        setMinimumHeight(400);

    }

    ~TestMainWindow()
    {
        killTimer(refreshTimerID);
    }

protected:

    void timerEvent(QTimerEvent *) override
    {
        meter->update();
    }

private:
    int refreshTimerID;
    AudioMeter *meter;
};


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMainWindow window;

    window.show();
    return app.exec();
}
