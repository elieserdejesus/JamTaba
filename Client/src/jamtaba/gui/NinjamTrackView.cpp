#include "NinjamTrackView.h"
#include "ui_TrackGroupView.h"
#include "BaseTrackView.h"
#include "ui_BaseTrackView.h"
#include <QLineEdit>
#include <QLabel>
#include <QDebug>
#include <QStyle>
#include "../MainController.h"

//+++++++++++++++++++++++++
NinjamTrackView::NinjamTrackView(Controller::MainController *mainController, long trackID, QString channelName)
    :BaseTrackView(mainController, trackID)
{
    channelNameLabel = new QLabel();
    channelNameLabel->setObjectName("channelName");
    channelNameLabel->setText(channelName);

    //channelNameLabel->setWordWrap(true);
    ui->mainLayout->insertSpacing(0, 12);
    ui->mainLayout->insertWidget(1, channelNameLabel);
}

void NinjamTrackView::setChannelName(QString name){
    this->channelNameLabel->setText(name);
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamTrackGroupView::updateGeoLocation(){
    Geo::Location location = mainController->getGeoLocation(this->userIP);
    countryLabel->setText("<img src=:/flags/flags/" + location.getCountryCode().toLower() +".png> <br>" + location.getCountryName());
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

NinjamTrackGroupView::NinjamTrackGroupView(QWidget *parent, Controller::MainController *mainController, long trackID, QString userName, QString channelName, QString userIP)
    :TrackGroupView(parent), mainController(mainController), userIP(userIP)
{
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
    updateGeoLocation();
    //countryLabel->setText("<img src=:/flags/flags/" + countryCode +".png> <br>" + countryName);

    ui->topPanel->layout()->addWidget(countryLabel);

    //create the first subchannel by default
    addTrackView(new NinjamTrackView(mainController, trackID, channelName));

    ui->groupNameField->setReadOnly(true);

}

void NinjamTrackGroupView::setNarrowStatus(bool narrow){
    foreach (BaseTrackView* trackView, trackViews) {
        if(narrow || trackViews.size() > 1){
            trackView->setToNarrow();
        }
        else{
            trackView->setToWide();
        }
    }
}

NinjamTrackGroupView::~NinjamTrackGroupView()
{

}

