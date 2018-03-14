#ifndef LOCAL_TRACK_VIEW_H
#define LOCAL_TRACK_VIEW_H

#include "BaseTrackView.h"
#include "gui/widgets/PeakMeter.h"

#include <QPushButton>

class FxPanel;

namespace audio {
class LocalInputNode;
}

class LooperWindow;

class LocalTrackView : public BaseTrackView
{
    Q_OBJECT

public:
    LocalTrackView(controller::MainController *mainController, int channelIndex);

    void setInitialValues(float initialGain, BaseTrackView::Boost boostValue, float initialPan, bool muted, bool stereoInverted);

    virtual ~LocalTrackView();

    void enableLopperButton(bool enabled);

    void updateStyleSheet() override;

    void closeAllPlugins();

    void detachMainController();

    int getInputIndex() const;

    audio::LocalInputNode *getInputNode() const;

    virtual void setActivatedStatus(bool unlighted);

    virtual void setPeakMetersOnlyMode(bool peakMetersOnly);
    void togglePeakMetersOnlyMode();

    bool isShowingPeakMetersOnly() const;

    QSize sizeHint() const;

    virtual void reset();

    void mute(bool b);
    void solo(bool b);
    void initializeBoostSpinBox(Boost boostValue);

    void setTintColor(const QColor &color) override;

signals:
    void openLooperEditor(uint trackIndex);

protected:
    audio::LocalInputNode *inputNode;

    QPushButton *buttonStereoInversion;
    QPushButton *buttonLooper;

    bool peakMetersOnly;

    virtual void setupMetersLayout();

    void bindThisViewWithTrackNodeSignals() override;

    void translateUI() override;

private:
    QPushButton *createStereoInversionButton();
    QPushButton *createLooperButton();

    bool inputIsUsedByThisTrack(int inputIndexInAudioDevice) const;
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
