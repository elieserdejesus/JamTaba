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
    ui->mainLayout->insertSpacing(0, 12);
    ui->mainLayout->insertWidget(1, channelNameLabel);
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
    //countryLabel->setStyleSheet("font-size: 7pt;");
    //countryLabel->setAlignment(Qt::AlignCenter);
    ui->topPanel->layout()->addWidget(countryLabel);

    //create the first subchannel by default
    addTrackView(new NinjamTrackView(mainController, trackID, channelName));

    ui->groupNameField->setReadOnly(true);

//    ui->channelName->setStyleSheet("font-size: 7pt");

//    //disable channel name, not editable
//    ui->channelName->setEnabled(false);

    //setEnabled(false);
    //setActivated(false);
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

