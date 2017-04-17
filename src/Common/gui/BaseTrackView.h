#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include <QWidget>
#include "audio/core/AudioPeak.h"
#include "MultiStateButton.h"
#include "Slider.h"

class AudioMeter;
class QLabel;
class QPushButton;
class QGroupBox;
class QSpacerItem;
class QVBoxLayout;
class QHBoxLayout;
class QBoxLayout;
class QGridLayout;

namespace Controller {
class MainController;
}

class BaseTrackView : public QWidget
{
    Q_OBJECT

public:
    BaseTrackView(Controller::MainController *mainController, long trackID);
    virtual ~BaseTrackView();

    enum Boost {
        ZERO, MINUS, PLUS
    };

    static Boost intToBoostValue(int intValue)
    {
        if (intValue == 0)
            return Boost::ZERO;
        if (intValue > 0)
            return Boost::PLUS;
        return Boost::MINUS;
    }

    inline long getTrackID() const
    {
        return trackID;
    }

    static BaseTrackView *getTrackViewByID(long trackID);

    virtual void setToNarrow();
    virtual void setToWide();

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    virtual void updateGuiElements();

    Controller::MainController *getMainController() const;

    virtual void setActivatedStatus(bool deactivated);

    bool isActivated() const;

    static const int NARROW_WIDTH;
    static const int WIDE_WIDTH;
protected:

    Controller::MainController *mainController;

    void paintEvent(QPaintEvent *) override;
    void changeEvent(QEvent *e) override;
    bool eventFilter(QObject *source, QEvent *ev);

    virtual void translateUI();

    long trackID;

    bool activated;
    bool narrowed;
    bool drawDbValue;

    virtual void setPeaks(float peakLeft, float peakRight, float rmsLeft, float rmsRight);

    // this is called in inherited classes [LocalTrackView, NinjamTrackView]
    virtual void bindThisViewWithTrackNodeSignals();

    void createLayoutStructure();

    virtual void refreshStyleSheet();

    virtual QPoint getDbValuePosition(const QString &dbValueText, const QFontMetrics &metrics) const;

    //meters
    AudioMeter *peakMeter;
    QBoxLayout *metersLayout;// used to group midi and audio meters

    //level slider
    Slider *levelSlider;
    QBoxLayout *levelSliderLayout;// used to group the level slider and the two 'speaker' icons

    // pan slider
    Slider *panSlider;
    QLabel *labelPanL;
    QLabel *labelPanR;
    QHBoxLayout *panWidgetsLayout;

    // mute solo
    QPushButton *muteButton;
    QPushButton *soloButton;
    QBoxLayout *muteSoloLayout;

    // boost
    MultiStateButton *buttonBoost;

    // main layout buildind blocks
    QGridLayout *mainLayout;
    QBoxLayout *secondaryChildsLayout; // right side widgets in vertical layout, bottom widgets (2nd row) in horizontal layout
    QBoxLayout *primaryChildsLayout; // left side widgets in vertical layout, top widgets (2nd row) in horizontal layout

    virtual void setupVerticalLayout();

    static const int FADER_HEIGHT;
private:
    static QMap<long, BaseTrackView *> trackViews;
    Audio::AudioPeak maxPeak;

    void drawFaderDbValue(QPainter &p);

    void updateBoostButtonToolTip();

    static const QColor DB_TEXT_COLOR;

protected slots:
    virtual void toggleMuteStatus();
    virtual void toggleSoloStatus();
    virtual void setGain(int value);
    virtual void setPan(int value);
    virtual void updateBoostValue();

private slots:
    // signals emitted by AudioNode class when user activate the control with mouse, or midi CCs, or using joystick, etc.
    void setPanKnobPosition(float newPanValue);
    void setGainSliderPosition(float newGainValue);
    void setMuteStatus(bool newMuteStatus);
    void setSoloStatus(bool newSoloStatus);
    void setBoostStatus(float newBoostValue);
};

inline Controller::MainController* BaseTrackView::getMainController() const
{
    return mainController;
}

#endif // TRACKVIEW_H
