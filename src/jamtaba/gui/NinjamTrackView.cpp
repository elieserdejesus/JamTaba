#include "NinjamTrackView.h"
#include "ui_BaseTrackView.h"

NinjamTrackView::NinjamTrackView(QWidget *parent, Controller::MainController *mainController, long trackID, QString userName, QString channelName, QString countryName, QString countryCode)
    :BaseTrackView(parent, mainController, trackID)
{
    this->setMaximumWidth(100);
    this->setMinimumWidth(100);

    ui->trackName->setText( userName );
    ui->channelName->setText(channelName);

    //country flag label
    QLabel* countryLabel = new QLabel(ui->namesPanel);
    countryLabel->setTextFormat(Qt::RichText);
    countryLabel->setText("<img src=:/flags/flags/" + countryCode +".png> " + countryName);
    countryLabel->setAlignment(Qt::AlignCenter);
    ui->namesPanel->layout()->addWidget(countryLabel);

    //disable channel name
    ui->channelName->setEnabled(true);
}

void NinjamTrackView::setUserName(QString newName){
    ui->trackName->setText(newName);
}

void NinjamTrackView::setChannelName(QString newChannelName){
    ui->channelName->setText(newChannelName);
}

NinjamTrackView::~NinjamTrackView()
{

}

