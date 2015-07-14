#include "MetronomeTrackView.h"
#include "ui_BaseTrackView.h"
#include "../MainController.h"
#include "../NinjamJamRoomController.h"

MetronomeTrackView::MetronomeTrackView(QWidget *parent, Controller::MainController *mainController, long metronomeID, float initialGain, float initialPan)
    :BaseTrackView(parent, mainController, metronomeID)
{
    //ui->channelName->setVisible(false);
    //ui->trackName->setTextFormat(Qt::RichText);
    //ui->trackName->setText("Metronome<br><img src=:/images/metronome.png>");

    setToNarrow();

    //set levelSlider position to initial gain
    ui->levelSlider->setValue((int)(100 * initialGain ));
    //mainController->setTrackLevel(Controller::NinjamJamRoomController::METRONOME_TRACK_ID, initialGain);
    //mainController->setTrackPan(Controller::NinjamJamRoomController::METRONOME_TRACK_ID, initialPan);

    //set pan slider to initial position [-4, 0, 4] = left, center. right
    ui->panSlider->setValue( initialPan * 4 );
}

MetronomeTrackView::~MetronomeTrackView()
{

}

