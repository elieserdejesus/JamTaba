#ifndef LOCALTRACKGROUPVIEW_H
#define LOCALTRACKGROUPVIEW_H

#include "TrackGroupView.h"
#include "LocalTrackView.h"

class QPushButton;
class MainWindow;

namespace Ui {
class LocalTrackGroupView;
}

class LocalTrackGroupView : public TrackGroupView
{
    Q_OBJECT
public:
    LocalTrackGroupView(int index, MainWindow *mainFrame);

    ~LocalTrackGroupView();

    QList<LocalTrackView *> getTracks() const;

    BaseTrackView *addTrackView(long trackID) override;

    inline int getChannelIndex() const
    {
        return index;
    }

    //void removeFxPanel();
    //void removeInputSelectionControls();

    virtual void setPeakMeterMode(bool peakMeterOnly);
    virtual void togglePeakMeterOnlyMode();
    bool isShowingPeakMeterOnly() const
    {
        return peakMeterOnly;
    }

    void detachMainControllerInSubchannels();
    void closePluginsWindows();

    void setToNarrow();
    void setToWide();

    void setPreparingStatus(bool preparing);// preparing to transmit
    inline bool isPreparingToTransmit() const
    {
        return preparingToTransmit;
    }

    void resetTracksControls();

protected:

    BaseTrackView *createTrackView(long trackID) override;

signals:
    void nameChanged();
    void trackRemoved();
    void trackAdded();
    void presetLoaded();
    void presetSaved();

private:
    QPushButton *toolButton;
    QPushButton *xmitButton;
    bool preparingToTransmit;

    static const int MAX_SUB_CHANNELS = 2;
    int index;
    MainWindow *mainFrame;

    bool peakMeterOnly;

    QPushButton *createToolButton();
    QPushButton *createXmitButton();

private slots:
    void showMenu();

    void addSubChannel();
    void addChannel();

    void highlightHoveredSubchannel();
    void highlightHoveredChannel();

    void removeSubchannel();
    void removeChannel();

    // PRESETS
    void loadPreset(QAction *a);
    void savePreset();
    void resetPreset();
    void deletePreset();

    // xmit
    void toggleTransmitingStatus(bool checked);
};

#endif // LOCALTRACKGROUPVIEW_H
