#include "NinjamTrackView.h"
#include "ui_BaseTrackView.h"

#include <QLineEdit>
#include <QLabel>
#include <QDebug>

NinjamTrackView::NinjamTrackView(QWidget *parent, Controller::MainController *mainController, long trackID, QString userName, QString channelName, QString countryName, QString countryCode)
    :BaseTrackView(parent, mainController, trackID)
{
    this->setMaximumWidth(100);
    this->setMinimumWidth(100);


    setUserName(userName);
    setChannelName(channelName);
    //ui->trackName->setText( userName );
    //ui->channelName->setText(channelName);

    //country flag label
    QLabel* countryLabel = new QLabel(ui->namesPanel);
    countryLabel->setObjectName("countryLabel");
    countryLabel->setTextFormat(Qt::RichText);
    countryLabel->setText("<img src=:/flags/flags/" + countryCode +".png> <br>" + countryName);
    countryLabel->setStyleSheet("font-size: 7pt;");
    countryLabel->setAlignment(Qt::AlignCenter);
    ui->namesPanel->layout()->addWidget(countryLabel);

    //disable channel name, not editable
    ui->channelName->setEnabled(false);
}

void NinjamTrackView::setUserName(QString newName){
    ui->trackName->setText( ui->trackName->fontMetrics().elidedText(newName, Qt::ElideRight, ui->trackName->width() * 0.8));
    ui->trackName->setToolTip(newName);
}

void NinjamTrackView::setChannelName(QString newChannelName){
    ui->channelName->setText(newChannelName);
    int textWidth = ui->channelName->fontMetrics().width(newChannelName);
    if(ui->channelName->contentsRect().width() < textWidth){
        ui->channelName->setAlignment(Qt::AlignLeft);
        ui->channelName->setToolTip(newChannelName);
        ui->channelName->home(false);//back to first character, user can read the first letter in channel name
    }
    else{
        ui->channelName->setAlignment(Qt::AlignHCenter);
        ui->channelName->setToolTip("");
    }
}

NinjamTrackView::~NinjamTrackView()
{

}

