#ifndef NINJAMTRACKVIEW_H
#define NINJAMTRACKVIEW_H

#include "TrackGroupView.h"
#include "BaseTrackView.h"
#include "widgets/IntervalChunksDisplay.h"

#include "widgets/MultiStateButton.h"
#include "persistence/UsersDataCache.h"
#include "audio/NinjamTrackNode.h"

namespace controller {
class MainController;
}

class QLabel;
class InstrumentsButton;

class NinjamTrackView : public BaseTrackView
{
    Q_OBJECT

public:
    NinjamTrackView(controller::MainController *mainController, long trackID);
    void setChannelName(const QString &name);
    void setInitialValues(const persistence::CacheEntry &initialValues);
    void setNinjamChannelData(const QString &userFullName, quint8 channelIndex);

    // interval chunks visual feedback
    void incrementDownloadedChunks(); // called when a interval part (a chunk) is received
    void finishCurrentDownload(); // called when the interval is fully downloaded
    void setEstimatedChunksPerInterval(int estimatedChunks); // how many download chunks per interval?

    void setActivatedStatus(bool deactivated) override;

    void updateGuiElements() override;

    QSize sizeHint() const override;

    void setOrientation(Qt::Orientation orientation);

    void updateStyleSheet() override;

    void setTintColor(const QColor &color) override;

    bool isVideoChannel() const;

    static void setNetworkUsageUpdatePeriod(quint32 periodInMilliseconds);

    void setChannelMode(NinjamTrackNode::ChannelMode mode);

protected:

    QPoint getDbValuePosition(const QString &dbValueText, const QFontMetrics &metrics) const override;

    void setPeaks(float peakLeft, float peakRight, float rmsLeft, float rmsRight) override;

    void setupVerticalLayout() override;
    void resizeEvent(QResizeEvent *ev) override;
    void paintEvent(QPaintEvent *ev) override;

private:
    MultiStateButton *buttonLowCut;
    QPushButton *buttonReceive;
    QHBoxLayout *networkUsageLayout;
    QLabel *networkUsageLabel;
    persistence::CacheEntry cacheEntry; // used to remember the track controls values
    IntervalChunksDisplay *chunksDisplay; // display downloaded interval chunks
    InstrumentsButton *instrumentsButton;

    QPixmap voiceChatIcon;

    // used to send channel receive on/off messages
    QString userFullName;
    quint8 channelIndex;

    Qt::Orientation orientation;

    void setupHorizontalLayout();

    void updateExtraWidgetsVisibility();

    bool downloadingFirstInterval;
    void setDownloadedChunksDisplayVisibility(bool visible);

    MultiStateButton *createLowCutButton(bool checked);

    void updateLowCutButtonToolTip();
    QString getLowCutStateText() const;

    QPushButton *createReceiveButton() const;

    NinjamTrackNode *getTrackNode() const;

    InstrumentsButton *createInstrumentsButton();

    qint8 guessInstrumentIcon() const;

    static const int WIDE_HEIGHT;

    qint64 lastNetworkUsageUpdate;

    static quint32 networkUsageUpdatePeriod;

    quint64 updateCounter = 0;

protected slots:
    // overriding the base class slots
    void toggleMuteStatus() override;
    void setGain(int value) override;
    void setPan(int value) override;
    void updateBoostValue(int) override;

    void setLowCutToNextState();

private slots:
    void setReceiveState(bool receive);
    void instrumentIconChanged(quint8 instrumentIndex);

};

#endif // NINJAMTRACKVIEW_H
