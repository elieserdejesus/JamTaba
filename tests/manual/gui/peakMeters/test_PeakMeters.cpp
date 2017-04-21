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
#include <QCheckBox>
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
        setStyleSheet("QFrame{ background-color: rgb(40, 40, 40); } QCheckBox{ color: white; } AudioMeter{ border: 1px solid black; min-width: 23px;}");

        QHBoxLayout *layout = new QHBoxLayout();
        AudioMeter *meter = new AudioMeter(this);
        meter->setMinimumWidth(20);
        meters.append(meter);

        layout->addWidget(meter);

        QVBoxLayout *buttonsLayout = new QVBoxLayout();
        float dbValues[] = {0.0, -6.0, -12.0, -24, 6.0, 12.0};
        for (float db : dbValues) {
            QPushButton *button = new QPushButton(QString::number(db) + " dB");
            connect(button, &QPushButton::clicked, [=](){
                float db = button->text().replace(" dB", "").toFloat();
                float peak = Utils::dbToLinear(db);
                for (AudioMeter *audioMeter : meters)
                    audioMeter->setPeak(peak, peak/2.0f, peak, peak/2.0f);

            });
            buttonsLayout->addWidget(button);
        }

        buttonsLayout->addSpacing(20);

        QCheckBox *stereoCheckBox = new QCheckBox("Stereo");
        stereoCheckBox->setChecked(true);
        buttonsLayout->addWidget(stereoCheckBox);

        QCheckBox *rmsCheckBox = new QCheckBox("RMS");
        rmsCheckBox->setChecked(AudioMeter::isPaintingRMS());
        buttonsLayout->addWidget(rmsCheckBox);
        connect(rmsCheckBox, &QCheckBox::clicked, [](bool checked){
            if (checked)
                AudioMeter::paintPeaksAndRms();
            else
                AudioMeter::paintPeaksOnly();
        });

        QCheckBox *peaksCheckbox = new QCheckBox("Peaks");
        peaksCheckbox->setChecked(AudioMeter::isPaintingPeaks());
        buttonsLayout->addWidget(peaksCheckbox);
        connect(peaksCheckbox, &QCheckBox::clicked, [=](bool checked) {
            if (checked) {
                if (rmsCheckBox->isChecked())
                    AudioMeter::paintPeaksAndRms();
                else
                    AudioMeter::paintPeaksOnly();
            }
            else {
                if (rmsCheckBox->isChecked())
                    AudioMeter::paintRmsOnly();
            }
        });

        buttonsLayout->addStretch(1);

        layout->addSpacing(20);

        AudioMeter *hMeter = new AudioMeter(this);
        hMeter->setOrientation(Qt::Horizontal);
        hMeter->setMaximumHeight(20);
        hMeter->setPaintMaxPeakMarker(true);
        meters.append(hMeter);

        layout->addWidget(hMeter, 1);

        layout->addLayout(buttonsLayout);

        setLayout(layout);

        for (AudioMeter *audioMeter : meters) {
            connect(stereoCheckBox, &QCheckBox::clicked, audioMeter, &AudioMeter::setStereo);
            audioMeter->setStereo(true);
            audioMeter->setPaintMaxPeakMarker(true);
            audioMeter->setMaxPeakColor(Qt::yellow);
        }

        setMinimumWidth(300);
        setMinimumHeight(200);

    }

    ~TestMainWindow()
    {
        killTimer(refreshTimerID);
    }

protected:

    void timerEvent(QTimerEvent *) override
    {
        for (AudioMeter *meter : meters)
            meter->update();
    }

private:
    int refreshTimerID;
    QList<AudioMeter *> meters;
};


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMainWindow window;

    window.show();
    return app.exec();
}
