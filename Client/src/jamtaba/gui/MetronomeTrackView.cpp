#include "MetronomeTrackView.h"
#include "ui_BaseTrackView.h"

MetronomeTrackView::MetronomeTrackView(QWidget *parent, Controller::MainController *mainController)
    :BaseTrackView(parent, mainController, -1)//-1 is the track ID for metronome
{
    ui->channelName->setVisible(false);
    ui->trackName->setTextFormat(Qt::RichText);
    ui->trackName->setText("Metronome<br><img src=:/images/metronome.png>");

    this->setMaximumWidth(80);

    ui->soloButton->setText("S");
    ui->soloButton->setToolTip("Solo");

    ui->muteButton->setText("M");
    ui->muteButton->setToolTip("Mute");

    ui->panSlider->setTickInterval(2);

}

MetronomeTrackView::~MetronomeTrackView()
{

}

