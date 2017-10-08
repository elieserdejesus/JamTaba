#ifndef LOCAL_TRACK_VIEW_H
#define LOCAL_TRACK_VIEW_H

#include "BaseTrackView.h"
#include "PeakMeter.h"
#include <QPushButton>

class FxPanel;

namespace Audio {
class LocalInputNode;
}

class LooperWindow;

class LocalTrackView : public BaseTrackView
{
    Q_OBJECT

public:
    LocalTrackView(Controller::MainController *mainController, int channelIndex);

    void setInitialValues(float initialGain, BaseTrackView::Boost boostValue, float initialPan, bool muted, bool stereoInverted);

    virtual ~LocalTrackView();

    void enableLopperButton(bool enabled);

    void updateStyleSheet() override;

    void closeAllPlugins();

    void detachMainController();

    int getInputIndex() const;

    Audio::LocalInputNode *getInputNode() const;

    virtual void setActivatedStatus(bool unlighted);

    virtual void setPeakMetersOnlyMode(bool peakMetersOnly);
    void togglePeakMetersOnlyMode();

    bool isShowingPeakMetersOnly() const;

    QSize sizeHint() const;

    virtual void reset();

    void mute(bool b);
    void solo(bool b);
    void initializeBoostSpinBox(Boost boostValue);

signals:
    void openLooperEditor(uint trackIndex);

protected:
    Audio::LocalInputNode *inputNode;

    QPushButton *buttonStereoInversion;
    QPushButton *buttonLooper;

    virtual void setupMetersLayout();

    void bindThisViewWithTrackNodeSignals() override;

    void translateUI() override;

private:
    QPushButton *createStereoInversionButton();
    QPushButton *createLooperButton();

    bool inputIsUsedByThisTrack(int inputIndexInAudioDevice) const;
    bool peakMetersOnly;
    void deleteWidget(QWidget *widget);

    class LooperIconFactory;

    static LooperIconFactory looperIconFactory;

private slots:
    void setStereoInversion(bool stereoInverted);
    void updateLooperButtonIcon();

};


inline bool LocalTrackView::isShowingPeakMetersOnly() const
{
    return peakMetersOnly;
}

inline int LocalTrackView::getInputIndex() const
{
    return getTrackID();
}

#endif
