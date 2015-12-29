#ifndef LOCALTRACKGROUPVIEW_H
#define LOCALTRACKGROUPVIEW_H

#include "TrackGroupView.h"

class LocalTrackView;
class MainWindow;
class QPushButton;

class LocalTrackGroupView : public TrackGroupView
{
    Q_OBJECT

public:
    LocalTrackGroupView(int index, MainWindow *mainFrame);

    virtual ~LocalTrackGroupView();

    QList<LocalTrackView *> getTracks() const;

    BaseTrackView *addTrackView(long trackID) override;

    inline int getChannelIndex() const
    {
        return index;
    }

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

    virtual void populateMenu(QMenu &menu);

    static const int MAX_SUB_CHANNELS = 2;

    MainWindow *mainFrame;

private:
    QPushButton *toolButton;
    QPushButton *xmitButton;
    bool preparingToTransmit;

    int index;

    bool peakMeterOnly;

    QPushButton *createToolButton();
    QPushButton *createXmitButton();

    QMenu* createPresetsSubMenu();
    void createPresetsActions(QMenu &menu);
    void createChannelsActions(QMenu &menu);

signals:
    void nameChanged();
    void trackRemoved();
    void trackAdded();
    void presetLoaded();
    void presetSaved();

private slots:
    void showMenu();

    void addSubChannel();
    void addChannel();

    void highlightHoveredSubchannel();
    void highlightHoveredChannel();

    void removeSubchannel();
    void removeChannel();

    void loadPreset(QAction *action);
    void savePreset();
    void resetPreset();
    void deletePreset();

    void toggleTransmitingStatus(bool checked);
};

#endif // LOCALTRACKGROUPVIEW_H
