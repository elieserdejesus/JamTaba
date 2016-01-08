#include "NinjamTrackGroupView.h"
#include "geo/IpToLocationResolver.h"
#include "MainController.h"

void NinjamTrackGroupView::updateGeoLocation()
{
    Geo::Location location = mainController->getGeoLocation(this->userIP);
    countryLabel->setText(
        "<img src=:/flags/flags/" + location.getCountryCode().toLower() +".png> <br>"
        + location.getCountryName());
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

NinjamTrackGroupView::NinjamTrackGroupView(QWidget *parent,
                                           Controller::MainController *mainController, long trackID,
                                           QString channelName,
                                           Persistence::CacheEntry initialValues) :
    TrackGroupView(parent),
    mainController(mainController),
    userIP(initialValues.getUserIP())
{
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));

    // change the top panel layout to vertical (original is horizontal)
    ui->topPanel->layout()->removeWidget(ui->groupNameField);
    delete ui->topPanel->layout();
    ui->topPanel->setLayout(new QVBoxLayout());
    ui->topPanel->layout()->setContentsMargins(3, 6, 3, 3);

    // replace the original QLineEdit with a MarqueeLabel

    groupNameLabel = new MarqueeLabel();
    delete ui->groupNameField;
    groupNameLabel->setObjectName("groupNameField");
    ui->topPanel->layout()->addWidget(groupNameLabel);

    setGroupName(initialValues.getUserName());

    // country flag label
    countryLabel = new QLabel();
    countryLabel->setObjectName("countryLabel");
    countryLabel->setTextFormat(Qt::RichText);
    updateGeoLocation();

    ui->topPanel->layout()->addWidget(countryLabel);

    // create the first subchannel by default
    NinjamTrackView *newTrackView = addTrackView(trackID);
    newTrackView->setChannelName(channelName);
    newTrackView->setInitialValues(initialValues);
}

NinjamTrackView *NinjamTrackGroupView::createTrackView(long trackID)
{
    return new NinjamTrackView(mainController, trackID);
}

void NinjamTrackGroupView::setGroupName(const QString &groupName)
{
    groupNameLabel->setText(groupName);
}

void NinjamTrackGroupView::setNarrowStatus(bool narrow)
{
    foreach (BaseTrackView *trackView, trackViews) {
        bool setToWide = !narrow && trackViews.size() <= 1;
        if (setToWide)
            trackView->setToWide();
        else
            trackView->setToNarrow();
    }
}

void NinjamTrackGroupView::updateGuiElements()
{
    TrackGroupView::updateGuiElements();
    groupNameLabel->updateMarquee();
}

NinjamTrackGroupView::~NinjamTrackGroupView()
{
}
