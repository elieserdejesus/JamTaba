#include <QApplication>

#include <QFrame>
#include <QLayout>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QDebug>
#include <QFile>
#include <QDateTime>

#include "gui/widgets/Slider.h"
#include "Utils.h"
#include "gui/ThemeLoader.h"

class TestMainWindow : public QFrame
{

public:
    TestMainWindow()
        :QFrame()
    {

        auto layout = new QHBoxLayout();

        auto verticalSlider = new AudioSlider();
        verticalSlider->setValue(80);
        verticalSlider->setOrientation(Qt::Vertical);
        layout->addWidget(verticalSlider);

        layout->addSpacing(32);

        auto horizontalSlider = new AudioSlider();
        horizontalSlider->setValue(80);
        horizontalSlider->setOrientation(Qt::Horizontal);
        layout->addWidget(horizontalSlider, 1);

        layout->addSpacing(32);

        auto panSlider = new PanSlider();
        panSlider->setMaximumWidth(100);
        panSlider->setOrientation(Qt::Horizontal);

        auto panLayout = new QHBoxLayout();
        panLayout->addWidget(new QLabel(QStringLiteral("L")));
        panLayout->addWidget(panSlider);
        panLayout->addWidget(new QLabel(QStringLiteral("R")));

        layout->addLayout(panLayout);

        setLayout(layout);

        setMinimumHeight(400);
        setMinimumWidth(800);

        refreshTimerID = startTimer(1000/30);

        sliders.append(verticalSlider);
        sliders.append(horizontalSlider);

        // --------------------------------------------

        auto buttonsLayout = new QVBoxLayout();
        float dbValues[] = {0.0, -6.0, -12.0, -24, 3.0, 6.0};
        for (float db : dbValues) {
            auto button = new QPushButton(QString::number(db) + " dB");
            connect(button, &QPushButton::clicked, [=](){
                float db = button->text().replace(" dB", "").toFloat();
                float peak = Utils::dbToLinear(db);
                for (auto slider : sliders)
                    slider->setPeak(peak, peak/2.0f, peak, peak/2.0f);

            });
            buttonsLayout->addWidget(button);
        }

        buttonsLayout->addSpacing(20);

        auto stereoCheckBox = new QCheckBox("Stereo");
        stereoCheckBox->setChecked(true);
        buttonsLayout->addWidget(stereoCheckBox);
        connect(stereoCheckBox, &QCheckBox::clicked, [=](){
            for (auto slider : sliders)
                slider->setStereo(!slider->isStereo());
        });

        auto rmsCheckBox = new QCheckBox("RMS");
        rmsCheckBox->setChecked(AudioSlider::isPaintingRMS());
        buttonsLayout->addWidget(rmsCheckBox);
        connect(rmsCheckBox, &QCheckBox::clicked, [](bool checked){
            if (checked)
                AudioSlider::paintPeaksAndRms();
            else
                AudioSlider::paintPeaksOnly();
        });

        auto peaksCheckbox = new QCheckBox("Peaks");
        peaksCheckbox->setChecked(AudioSlider::isPaintingPeaks());
        buttonsLayout->addWidget(peaksCheckbox);
        connect(peaksCheckbox, &QCheckBox::clicked, [=](bool checked) {
            if (checked) {
                if (rmsCheckBox->isChecked())
                    AudioSlider::paintPeaksAndRms();
                else
                    AudioSlider::paintPeaksOnly();
            }
            else {
                if (rmsCheckBox->isChecked())
                    AudioSlider::paintRmsOnly();
            }
        });

        auto randomPeaksCheckBox = new QCheckBox("Random peaks");
        randomPeaksCheckBox->setChecked(generatingRandomPeaks);
        connect(randomPeaksCheckBox, &QCheckBox::clicked, [=](){
            generatingRandomPeaks = !generatingRandomPeaks;
        });
        buttonsLayout->addWidget(randomPeaksCheckBox);

        auto paintMaxPeaksCheckBox = new QCheckBox("Paint max peaks");
        paintMaxPeaksCheckBox->setChecked(AudioSlider::isPaintintMaxPeakMarker());
        connect(paintMaxPeaksCheckBox, &QCheckBox::clicked, [=](){
            AudioSlider::setPaintMaxPeakMarker(!AudioSlider::isPaintintMaxPeakMarker());
        });
        buttonsLayout->addWidget(paintMaxPeaksCheckBox);

        layout->addLayout(buttonsLayout);

        rmsCheckBox->click();
        stereoCheckBox->click();
    }

protected:
    void timerEvent(QTimerEvent *) override
    {
        static auto lastPeakUpdate = QDateTime::currentMSecsSinceEpoch();

        auto now = QDateTime::currentMSecsSinceEpoch();
        auto canGeneratePeaks = (now - lastPeakUpdate) >= 500;
        auto peak = 0.0;
        if (canGeneratePeaks) {
            peak = (qrand() % 100)/100.0;
            if (peak < 0.35)
                peak = 0.35;
            lastPeakUpdate = now;
        }

        for (auto slider : sliders) {
            if (generatingRandomPeaks) {
                slider->setPeak(peak, peak, peak, peak);
            }
            slider->update();
        }
    }

private:
    int refreshTimerID;
    QList<AudioSlider *> sliders;
    bool generatingRandomPeaks = true;
};


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    auto css = theme::Loader::loadCSS("E:\\Jamtaba2\\JamTaba\\src\\resources\\css\\themes", "Navy_nm");
    app.setStyleSheet(css);

    TestMainWindow window;

    window.show();
    return app.exec();
}
