#include "TrackGroupView.h"
#include "BaseTrackView.h"
#include <QPainter>
#include <QDebug>

TrackGroupView::TrackGroupView(QWidget *parent) :
    QWidget(parent)
{
    setupUI();
}

void TrackGroupView::setupUI()
{
    setObjectName(QStringLiteral("TrackGroupView"));
    setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred));

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(1, 1, 1, 1);

    topPanel = new QWidget(this);
    topPanel->setObjectName(QStringLiteral("topPanel"));

    groupNameField = new QLineEdit(topPanel);
    groupNameField->setObjectName(QStringLiteral("groupNameField"));
    groupNameField->setAttribute(Qt::WA_MacShowFocusRect, 0);// disable blue border when QLineEdit has focus in mac

    topPanelLayout = new QHBoxLayout(topPanel);
    topPanelLayout->setSpacing(3);
    topPanelLayout->setContentsMargins(6, 12, 6, 12);
    topPanelLayout->addWidget(groupNameField);

    tracksLayout = new QHBoxLayout();
    tracksLayout->setContentsMargins(0, 0, 0, 0);
    tracksLayout->setSpacing(1);

    mainLayout->addWidget(topPanel);
    mainLayout->addLayout(tracksLayout, 1);
}

bool TrackGroupView::isUnlighted() const
{
    return property("unlighted").toBool();
}

void TrackGroupView::refreshStyleSheet()
{
    style()->unpolish(this);
    style()->polish(this);

    style()->unpolish(topPanel);
    style()->polish(topPanel);
}

void TrackGroupView::setUnlightStatus(bool unlighted)
{
    setProperty("unlighted", QVariant(unlighted));
    refreshStyleSheet();

    foreach (BaseTrackView *localTrack, trackViews)
        localTrack->setUnlightStatus(unlighted);
    update();
}

void TrackGroupView::updateGuiElements()
{
    foreach (BaseTrackView *trackView, trackViews)
        trackView->updateGuiElements();
}

TrackGroupView::~TrackGroupView()
{
    //delete ui;
}

// little to allow stylesheet in custom widget
void TrackGroupView::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

BaseTrackView *TrackGroupView::addTrackView(long trackID)
{
    BaseTrackView* newTrackView = createTrackView(trackID);//this is a factory method and is overrided in some places
    if (tracksLayout) {
        tracksLayout->addWidget(newTrackView);
        trackViews.append(newTrackView);

        if (trackViews.size() > 1) {
            foreach (BaseTrackView *trackView, trackViews)
                trackView->setToNarrow();
            updateGeometry();
            newTrackView->setUnlightStatus(isUnlighted());
        }
    }
    return newTrackView;
}

void TrackGroupView::setGroupName(const QString &groupName)
{
    groupNameField->setText(groupName);
}

QString TrackGroupView::getGroupName() const
{
    return groupNameField->text();
}

// +++++++++++++++++++++++++++++++++++++++++
void TrackGroupView::removeTrackView(BaseTrackView *trackView)
{
    tracksLayout->removeWidget(trackView);
    trackViews.removeOne(trackView);
    trackView->deleteLater();
    if (trackViews.size() == 1)
        trackViews.at(0)->setToWide();
    updateGeometry();
}

void TrackGroupView::removeTrackView(int trackIndex)
{
    if (trackIndex >= 0 && trackIndex < trackViews.size())
        removeTrackView(trackViews.at(trackIndex));
    else
        qCritical() << "Invalid index " << trackIndex;
}

// +++++++++++++++++++++++++++++++++++++++++

QSize TrackGroupView::minimumSizeHint() const
{
    return sizeHint();
}

QSize TrackGroupView::sizeHint() const
{
    int width = 0;
    foreach (BaseTrackView *trackView, trackViews)
        width += trackView->minimumSizeHint().width();
    return QSize(width, 10);
}
