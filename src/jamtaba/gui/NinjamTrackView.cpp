#include "NinjamTrackView.h"
#include "ui_BaseTrackView.h"

NinjamTrackView::NinjamTrackView(QWidget *parent, Controller::MainController *mainController, int trackID, QString userName, QString channelName, QString countryName, QString countryCode)
    :BaseTrackView(parent, mainController, trackID)
{
    this->setMaximumWidth(100);
    ui->trackName->setText( userName );
    ui->channelName->setText(channelName);

    //country flag label
    QLabel* countryLabel = new QLabel(ui->namesPanel);
    countryLabel->setTextFormat(Qt::RichText);
    countryLabel->setText("<img src=:/flags/flags/" + countryCode +".png> " + countryName);
    countryLabel->setAlignment(Qt::AlignCenter);
    ui->namesPanel->layout()->addWidget(countryLabel);

    //disable channel name
    ui->channelName->setEnabled(false);
}

NinjamTrackView::~NinjamTrackView()
{

}

