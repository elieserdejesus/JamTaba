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
    userIP(initialValues.getUserIP()),
    orientation(Qt::Vertical)
{
    setupVerticalLayout();

    // change the top panel layout to vertical (original is horizontal)
    topPanelLayout->setDirection(QHBoxLayout::TopToBottom);
    topPanelLayout->setContentsMargins(1, 1, 1, 1);
    topPanelLayout->setSpacing(0);

    // replace the original QLineEdit with a MarqueeLabel
    topPanelLayout->removeWidget(groupNameField);
    delete groupNameField;
    groupNameLabel = new MarqueeLabel();
    groupNameLabel->setObjectName("groupNameField");
    groupNameLabel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum));
    topPanelLayout->addWidget(groupNameLabel);
    topPanelLayout->setAlignment(groupNameLabel, Qt::AlignBottom);

    setGroupName(initialValues.getUserName());

    // country flag label
    countryLabel = new QLabel();
    countryLabel->setObjectName("countryLabel");
    countryLabel->setTextFormat(Qt::RichText);
    countryLabel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum));
    updateGeoLocation();
    topPanelLayout->addWidget(countryLabel);
    topPanelLayout->setAlignment(countryLabel, Qt::AlignTop);

    // create the first subchannel by default
    NinjamTrackView *newTrackView = addTrackView(trackID);
    newTrackView->setChannelName(channelName);
    newTrackView->setInitialValues(initialValues);
}

void NinjamTrackGroupView::setOrientation(Qt::Orientation newOrientation)
{
    if(newOrientation == orientation)
        return;

    orientation = newOrientation;
    QList<NinjamTrackView *> tracks = getTracks<NinjamTrackView *>();
    foreach (NinjamTrackView *track, tracks) {
        track->setOrientation(newOrientation);
    }

    if(newOrientation == Qt::Horizontal){
        setupHorizontalLayout();
        topPanel->setFixedWidth(100);//using fixed width in horizontal layout
    }
    else{
        setupVerticalLayout();
        topPanel->setMaximumWidth(QWIDGETSIZE_MAX);
        topPanel->setMinimumWidth(1);
    }

    setProperty("horizontal", newOrientation == Qt::Horizontal ? true : false);
    refreshStyleSheet();

    updateGeometry();
}

void NinjamTrackGroupView::setupHorizontalLayout()
{
    tracksLayout->setDirection(QHBoxLayout::TopToBottom);
    mainLayout->setDirection(QHBoxLayout::LeftToRight);
    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));
}

void NinjamTrackGroupView::setupVerticalLayout()
{
    tracksLayout->setDirection(QHBoxLayout::LeftToRight);
    mainLayout->setDirection(QHBoxLayout::TopToBottom);
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
}

NinjamTrackView *NinjamTrackGroupView::createTrackView(long trackID)
{
    return new NinjamTrackView(mainController, trackID);
}

void NinjamTrackGroupView::setGroupName(const QString &groupName)
{
    groupNameLabel->setText(groupName);
}

QSize NinjamTrackGroupView::sizeHint() const
{
    if(orientation == Qt::Vertical )
        return TrackGroupView::sizeHint();

    if(trackViews.size() > 1){
        int height = 0;
        foreach (BaseTrackView *trackView, trackViews)
            height += trackView->minimumSizeHint().height();
        return QSize(1, height);
    }
    return QSize(1, 54);
}

NinjamTrackView *NinjamTrackGroupView::addTrackView(long trackID)
{
    NinjamTrackView *newTrackView = dynamic_cast<NinjamTrackView *>(TrackGroupView::addTrackView(trackID));
    newTrackView->setOrientation(this->orientation);
    return newTrackView;
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
