#include "TrackGroupView.h"
#include "ui_TrackGroupView.h"
#include "LocalTrackView.h"
#include <QPainter>
#include <QDebug>

TrackGroupView::TrackGroupView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrackGroupView)
{
    ui->setupUi(this);

    ui->tracksPanel->layout()->setContentsMargins(0, 0, 0, 0);
    ui->tracksPanel->layout()->setSpacing(1);

    // disable blue border when QLineEdit has focus in mac
    ui->groupNameField->setAttribute(Qt::WA_MacShowFocusRect, 0);
}

bool TrackGroupView::isUnlighted() const
{
    return property("unlighted").toBool();
}

void TrackGroupView::setUnlightStatus(bool unlighted)
{
    setProperty("unlighted", QVariant(unlighted));
    style()->unpolish(this);
    style()->polish(this);

    style()->unpolish(ui->topPanel);
    style()->polish(ui->topPanel);

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
    delete ui;
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
    if (ui->tracksPanel->layout()) {
        ui->tracksPanel->layout()->addWidget(newTrackView);
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
    ui->groupNameField->setText(groupName);
}

QString TrackGroupView::getGroupName() const
{
    return ui->groupNameField->text();
}

// +++++++++++++++++++++++++++++++++++++++++
void TrackGroupView::removeTrackView(BaseTrackView *trackView)
{
    ui->tracksPanel->layout()->removeWidget(trackView);
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
