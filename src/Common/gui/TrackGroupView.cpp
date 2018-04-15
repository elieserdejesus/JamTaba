#include "TrackGroupView.h"
#include "BaseTrackView.h"
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QEvent>
#include <QDebug>
#include <QMenu>
#include <QActionGroup>
#include "widgets/PeakMeter.h"

TrackGroupView::TrackGroupView(TextEditorModifier *TextEditorModifier, QWidget *parent) :
    QFrame(parent)
{
    setupUI(TextEditorModifier);

    // context menu
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &TrackGroupView::customContextMenuRequested, this, &TrackGroupView::showContextMenu);
}

void TrackGroupView::setTintColor(const QColor &color)
{
    for (auto trackGroup : trackViews) {
        trackGroup->setTintColor(color);
    }
}

void TrackGroupView::populateContextMenu(QMenu &contextMenu)
{
    Q_UNUSED(contextMenu);
    // no common actions at moment
}

void TrackGroupView::showContextMenu(const QPoint &pos){
    QMenu contextMenu(this);
    populateContextMenu(contextMenu); // populate is overrided in subclasses to add more menu actions
    contextMenu.exec(mapToGlobal(pos));
}

void TrackGroupView::showPeakMeterOnly()
{
    AudioSlider::paintPeaksOnly();
}

void TrackGroupView::showRmsOnly()
{
    AudioSlider::paintRmsOnly();
}

void TrackGroupView::showPeakAndRms()
{
    AudioSlider::paintPeaksAndRms();
}

void TrackGroupView::showMaxPeakMarker(bool showMarker)
{
    AudioSlider::setPaintMaxPeakMarker(showMarker);
}

void TrackGroupView::setupUI(TextEditorModifier *textEditorModifier)
{
    setObjectName(QStringLiteral("TrackGroupView"));
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    mainLayout = new QGridLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(3, 3, 3, 3);

    topPanel = new QWidget(this);
    topPanel->setObjectName(QStringLiteral("topPanel"));

    groupNameField = new QLineEdit(topPanel);
    groupNameField->setObjectName(QStringLiteral("groupNameField"));
    groupNameField->setAttribute(Qt::WA_MacShowFocusRect, 0); // disable blue border when QLineEdit has focus in mac

    topPanelLayout = new QHBoxLayout();
    topPanelLayout->setObjectName(QStringLiteral("topPanelLayout"));
    topPanelLayout->setSpacing(3);
    topPanelLayout->setContentsMargins(6, 12, 6, 12);

    topPanelLayout->addWidget(groupNameField);

    if (textEditorModifier) {
        bool finishEditorPressingReturnKey = true;
        textEditorModifier->modify(groupNameField, finishEditorPressingReturnKey);
    }

    tracksLayout = new QHBoxLayout();
    tracksLayout->setContentsMargins(0, 3, 0, 0); // adding a small top margin to fix #861
    tracksLayout->setSpacing(1);
    tracksLayout->setObjectName(QStringLiteral("tracksLayout"));

    topPanel->setLayout(topPanelLayout);

    mainLayout->addWidget(topPanel, 0, 0, 1, 1);
    mainLayout->addLayout(tracksLayout, 1, 0, 1, 1);

    mainLayout->setRowStretch(1, 1);

    translateUi();
}

void TrackGroupView::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        translateUi();
    }
    QFrame::changeEvent(e);
}

void TrackGroupView::translateUi()
{
    // overrided in subclasses
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
        localTrack->setActivatedStatus(unlighted);
    update();
}

void TrackGroupView::updateGuiElements()
{
    for (BaseTrackView *trackView : trackViews)
        trackView->updateGuiElements();
}

TrackGroupView::~TrackGroupView()
{
    //
}


BaseTrackView *TrackGroupView::addTrackView(long trackID)
{
    BaseTrackView* newTrackView = createTrackView(trackID); // this is a factory method and is overrided in some places
    if (tracksLayout) {
        tracksLayout->addWidget(newTrackView);
        trackViews.append(newTrackView);

        if (trackViews.size() > 1) {
            for (BaseTrackView *trackView : trackViews)
                trackView->setToNarrow();

            updateGeometry();
            newTrackView->setActivatedStatus(isUnlighted());
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

QSize TrackGroupView::minimumSizeHint() const
{
    return sizeHint();
}

QSize TrackGroupView::sizeHint() const
{
    int width = 0;

    for (BaseTrackView *trackView : trackViews)
        width += trackView->minimumSizeHint().width();

    return QSize(width, 10);
}
