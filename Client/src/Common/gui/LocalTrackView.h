#ifndef LOCAL_TRACK_VIEW_H
#define LOCAL_TRACK_VIEW_H

#include "BaseTrackView.h"
#include "PeakMeter.h"
#include "audio/core/AudioNode.h"

class FxPanel;

class LocalTrackView : public BaseTrackView
{
    Q_OBJECT

public:
    LocalTrackView(Controller::MainController *mainController, int channelIndex);

    void setInitialValues(float initialGain, BaseTrackView::Boost boostValue, float initialPan, bool muted);

    virtual ~LocalTrackView();

    void closeAllPlugins();

    void detachMainController();

    inline int getInputIndex() const
    {
        return getTrackID();
    }

    Audio::LocalInputAudioNode *getInputNode() const;

    virtual void setUnlightStatus(bool unlighted);

    virtual void setPeakMetersOnlyMode(bool peakMetersOnly, bool runningInMiniMode);
    void togglePeakMetersOnlyMode(bool runninsInMiniMode);

    QSize sizeHint() const;

    virtual void reset();

    void mute(bool b);
    void solo(bool b);
    void initializeBoostButtons(Boost boostValue);

protected:
    Audio::LocalInputAudioNode *inputNode;

private:
    bool inputIsUsedByThisTrack(int inputIndexInAudioDevice) const;
    bool peakMetersOnly;
    void deleteWidget(QWidget *widget);
};

#endif
