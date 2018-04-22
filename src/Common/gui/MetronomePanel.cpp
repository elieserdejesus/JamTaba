#include "MetronomePanel.h"
#include "ui_MetronomePanel.h"
#include "IconFactory.h"
#include "BaseTrackView.h"

MetronomePanel::MetronomePanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MetronomePanel)
{
    ui->setupUi(this);

    ui->levelSlider->setOrientation(Qt::Horizontal);

    setupSignals();
}

MetronomePanel::~MetronomePanel()
{
    delete ui;
}

void MetronomePanel::updateStyleSheet()
{
    ui->levelSlider->updateStyleSheet();
}

void MetronomePanel::setupSignals()
{
    connect(ui->levelSlider, &QSlider::valueChanged, this, &MetronomePanel::gainSliderChanged);
    connect(ui->panSlider, &QSlider::valueChanged, this, &MetronomePanel::panSliderChanged);
    connect(ui->muteButton, &QPushButton::clicked, this, &MetronomePanel::muteButtonClicked);
    connect(ui->soloButton, &QPushButton::clicked, this, &MetronomePanel::soloButtonClicked);

    connect(ui->preferencesButton, &QPushButton::clicked, this, &MetronomePanel::preferencesButtonClicked);
    connect(ui->floatingWindowButton, &QPushButton::toggled, this, &MetronomePanel::floatingWindowButtonToggled);

}

void MetronomePanel::setFloatingWindowButtonChecked(bool checked)
{
    ui->floatingWindowButton->setChecked(checked);
}

int MetronomePanel::getPanSliderMaximumValue() const
{
    return ui->panSlider->maximum();
}

int MetronomePanel::getGainSliderMaximumValue() const
{
    return ui->levelSlider->maximum();
}

void MetronomePanel::setMuteButtonStatus(bool checked)
{
    ui->muteButton->setChecked(checked);
}

void MetronomePanel::setPanSliderValue(int value)
{
    ui->panSlider->setValue(value);
}

void MetronomePanel::setGainSliderValue(int value)
{
    ui->levelSlider->setValue(value);
}

void MetronomePanel::setMetronomePeaks(float left, float right, float rmsLeft, float rmsRight)
{
    ui->levelSlider->setPeak(left, right, rmsLeft, rmsRight);
}
