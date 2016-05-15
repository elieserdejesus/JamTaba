#ifndef LOCAL_TRACK_VIEW_H
#define LOCAL_TRACK_VIEW_H

#include "BaseTrackView.h"
#include "PeakMeter.h"
#include <QPushButton>

class FxPanel;

namespace Audio {
class LocalInputNode;
}

class LocalTrackView : public BaseTrackView
{
    Q_OBJECT

public:
    LocalTrackView(Controller::MainController *mainController, int channelIndex);

    void setInitialValues(float initialGain, BaseTrackView::Boost boostValue, float initialPan, bool muted, bool stereoInverted);

    virtual ~LocalTrackView();

    void closeAllPlugins();

    void detachMainController();

    inline int getInputIndex() const
    {
        return getTrackID();
    }

    Audio::LocalInputNode *getInputNode() const;

    virtual void setUnlightStatus(bool unlighted);

    virtual void setPeakMetersOnlyMode(bool peakMetersOnly, bool runningInMiniMode);
    void togglePeakMetersOnlyMode(bool runninsInMiniMode);

    inline bool isShowingPeakMetersOnly() const { return peakMetersOnly; }

    QSize sizeHint() const;

    virtual void reset();

    void mute(bool b);
    void solo(bool b);
    void initializeBoostButtons(Boost boostValue);

protected:
    Audio::LocalInputNode *inputNode;
    QPushButton *buttonStereoInversion;

    void refreshStyleSheet() override;

private:

    QPushButton *createStereoInversionButton();

    bool inputIsUsedByThisTrack(int inputIndexInAudioDevice) const;
    bool peakMetersOnly;
    void deleteWidget(QWidget *widget);

private slots:
    void setStereoInversion(bool stereoInverted);

};

#endif
