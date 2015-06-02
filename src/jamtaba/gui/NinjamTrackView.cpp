#include "NinjamTrackView.h"
#include "ui_BaseTrackView.h"

NinjamTrackView::NinjamTrackView(QWidget *parent, Controller::MainController *mainController, int trackID, QString channelName, Login::NinjamPeer* ninjamPeer)
    :BaseTrackView(parent, mainController, trackID)
{
    this->setMaximumWidth(100);
    ui->trackName->setText(ninjamPeer->getName());
    ui->channelName->setText(channelName);

    //country flag label
    QLabel* countryLabel = new QLabel(ui->namesPanel);
    countryLabel->setTextFormat(Qt::RichText);
    QString countryCode = ninjamPeer->getCountryCode().toLower();
    QString countryName = ninjamPeer->getCountryName();
    countryLabel->setText("<img src=:/flags/flags/" + countryCode +".png> " + countryName);
    countryLabel->setAlignment(Qt::AlignCenter);
    ui->namesPanel->layout()->addWidget(countryLabel);

    //disable channel name
    ui->channelName->setEnabled(false);
}

NinjamTrackView::~NinjamTrackView()
{

}

