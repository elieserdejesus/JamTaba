#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include <QWidget>
#include <QVariant>
#include <QStyle>
#include "audio/core/AudioPeak.h"

namespace Ui {
class BaseTrackView;
}

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

    inline Controller::MainController *getMainController() const
    {
        return mainController;
    }

    virtual void setUnlightStatus(bool unlighted);

    static const int NARROW_WIDTH;
    static const int WIDE_WIDTH;
protected:

    Ui::BaseTrackView *ui;
    Controller::MainController *mainController;

    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *source, QEvent *ev);

    long trackID;

    bool activated;
    bool narrowed;
    bool drawDbValue;

    void setPeaks(float left, float right);

    // this is called in inherited classes [LocalTrackView, NinjamTrackView]
    void bindThisViewWithTrackNodeSignals();
private:
    static QMap<long, BaseTrackView *> trackViews;
    Audio::AudioPeak maxPeak;

    void drawFaderDbValue(QPainter &p);

    static const QColor DB_TEXT_COLOR;
    static const int FADER_HEIGHT;

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
};

#endif // TRACKVIEW_H
