#ifndef LOCAL_TRACK_VIEW_H
#define LOCAL_TRACK_VIEW_H

#include "BaseTrackView.h"
#include <QWidget>
#include <QTimer>
#include "PeakMeter.h"

class QMenu;
class FxPanel;
class QFrame;
class QPushButton;
class QLabel;
class QSpacerItem;

namespace Audio {
class Plugin;
class LocalInputAudioNode;
}

namespace Controller {
class MainController;
}

class LocalTrackView : public BaseTrackView
{
    Q_OBJECT
public:
    //LocalTrackView(Controller::MainController *mainController, int channelIndex, float initialGain,
      //             BoostValue boostValue, float initialPan, bool muted);
    LocalTrackView(Controller::MainController *mainController, int channelIndex);


    void setInitialValues(float initialGain, BaseTrackView::BoostValue boostValue, float initialPan, bool muted);

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
    void initializeBoostButtons(BoostValue boostValue);

protected:
    Audio::LocalInputAudioNode *inputNode;

private:
    bool inputIsUsedByThisTrack(int inputIndexInAudioDevice) const;
    bool peakMetersOnly;
    void deleteWidget(QWidget *widget);
};

#endif
