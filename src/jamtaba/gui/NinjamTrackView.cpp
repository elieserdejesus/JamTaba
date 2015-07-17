#include "NinjamTrackView.h"
#include "ui_TrackGroupView.h"
#include "BaseTrackView.h"
#include "ui_BaseTrackView.h"
#include <QLineEdit>
#include <QLabel>
#include <QDebug>
#include <QStyle>

//+++++++++++++++++++++++++
NinjamTrackView::NinjamTrackView(Controller::MainController *mainController, long trackID, QString channelName)
    :BaseTrackView(mainController, trackID)
{
    channelNameLabel = new QLabel();
    channelNameLabel->setObjectName("channelName");
    channelNameLabel->setText(channelName);

    //channelNameLabel->setWordWrap(true);
    ui->mainLayout->insertWidget(0, channelNameLabel);
}

void NinjamTrackView::setChannelName(QString name){
    this->channelNameLabel->setText(name);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
NinjamTrackGroupView::NinjamTrackGroupView(QWidget *parent, Controller::MainController *mainController, long trackID, QString userName, QString channelName, QString countryName, QString countryCode)
    :TrackGroupView(parent), mainController(mainController)
{
    //this->setMaximumWidth(100);
    //this->setMinimumWidth(100);

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));

    //change the top panel layout to vertical (original is horizontal)
    ui->topPanel->layout()->removeWidget(ui->groupNameField);
    delete ui->topPanel->layout();
    ui->topPanel->setLayout(new QVBoxLayout());
    ui->topPanel->layout()->addWidget(ui->groupNameField);

    setGroupName(userName);

    //country flag label
    countryLabel = new QLabel();
    countryLabel->setObjectName("countryLabel");
    countryLabel->setTextFormat(Qt::RichText);
    countryLabel->setText("<img src=:/flags/flags/" + countryCode +".png> <br>" + countryName);
    countryLabel->setStyleSheet("font-size: 7pt;");
    countryLabel->setAlignment(Qt::AlignCenter);
    ui->topPanel->layout()->addWidget(countryLabel);

    //create the first subchannel by default
    addTrackView(new NinjamTrackView(mainController, trackID, channelName));

//    ui->channelName->setStyleSheet("font-size: 7pt");

//    //disable channel name, not editable
//    ui->channelName->setEnabled(false);

    //setEnabled(false);
    //setActivated(false);
}

//void NinjamTrackView::setUserName(QString newName){
//    ui->trackName->setText( ui->trackName->fontMetrics().elidedText(newName, Qt::ElideRight, ui->trackName->width() * 0.8));
//    ui->trackName->setToolTip(newName);
//}

//void NinjamTrackView::setChannelName(QString newChannelName){
//    ui->channelName->setText(newChannelName);
//    int textWidth = ui->channelName->fontMetrics().width(newChannelName);
//    if(ui->channelName->contentsRect().width() < textWidth){
//        ui->channelName->setAlignment(Qt::AlignLeft);
//        ui->channelName->setToolTip(newChannelName);
//        ui->channelName->home(false);//back to first character, user can read the first letter in channel name
//    }
//    else{
//        ui->channelName->setAlignment(Qt::AlignHCenter);
//        ui->channelName->setToolTip("");
//    }
//}

NinjamTrackGroupView::~NinjamTrackGroupView()
{

}

