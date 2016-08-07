#ifndef NINJAMTRACKVIEW_H
#define NINJAMTRACKVIEW_H

#include "TrackGroupView.h"
#include "BaseTrackView.h"
#include "IntervalChunksDisplay.h"
#include "persistence/UsersDataCache.h"
#include "MarqueeLabel.h"

namespace Controller {
class MainController;
}

class QLabel;

class NinjamTrackView : public BaseTrackView
{
    Q_OBJECT
public:
    NinjamTrackView(Controller::MainController *mainController, long trackID);
    void setChannelName(const QString &name);
    void setInitialValues(const Persistence::CacheEntry &initialValues);

    // interval chunks visual feedback
    void incrementDownloadedChunks();// called when a interval part (a chunk) is received
    void finishCurrentDownload(); // called when the interval is fully downloaded
    void setEstimatedChunksPerInterval(int estimatedChunks);// how many download chunks per interval?

    void setUnlightStatus(bool unlighted);

    void updateGuiElements();

    QSize sizeHint() const override;

    void setOrientation(Qt::Orientation orientation);

protected:
    void refreshStyleSheet() override;
    QPoint getDbValuePosition(const QString &dbValueText, const QFontMetrics &metrics) const override;

    void setupVerticalLayout() override;

private:
    MarqueeLabel *channelNameLabel;
    QPushButton *buttonLowCut;
    Persistence::CacheEntry cacheEntry;// used to remember the track controls values
    IntervalChunksDisplay *chunksDisplay;// display downloaded interval chunks

    Qt::Orientation orientation;

    void setupHorizontalLayout();

    bool downloadingFirstInterval;
    void setDownloadedChunksDisplayVisibility(bool visible);

    QPushButton *createLowCutButton(bool checked);

    static const int WIDE_HEIGHT;

protected slots:
    // overriding the base class slots
    void toggleMuteStatus() override;
    void setGain(int value) override;
    void setPan(int value) override;
    void updateBoostValue() override;

    void setLowCutStatus(bool activated);
};

#endif // NINJAMTRACKVIEW_H
