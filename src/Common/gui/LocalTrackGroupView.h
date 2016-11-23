#ifndef LOCALTRACKGROUPVIEW_H
#define LOCALTRACKGROUPVIEW_H

#include "TrackGroupView.h"
#include "LocalTrackView.h"

class MainWindow;
class QPushButton;

class LocalTrackGroupView : public TrackGroupView
{
    Q_OBJECT

public:
    LocalTrackGroupView(int index, MainWindow *mainFrame);

    virtual ~LocalTrackGroupView();

    LocalTrackView *addTrackView(long trackID) override;

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

    void resetTracks();

    void useSmallSpacingInLayouts(bool useSmallSpacing);

protected:

    LocalTrackView *createTrackView(long trackID) override;

    void translateUi() override;

    virtual void populateMenu(QMenu &menu);

    void refreshStyleSheet() override;

    static const int MAX_SUB_CHANNELS = 2;

    MainWindow *mainFrame;

private:
    QPushButton *toolButton;
    QPushButton *xmitButton;
    bool preparingToTransmit;
    bool usingSmallSpacingInLayouts;

    int index;

    bool peakMeterOnly;

    QPushButton *createToolButton();
    QPushButton *createXmitButton();

    QMenu* createPresetsLoadingSubMenu();
    QMenu* createPresetsDeletingSubMenu();
    void createPresetsActions(QMenu &menu);
    void createChannelsActions(QMenu &menu);

    void updateXmitButtonText();

    static QString getStripedPresetName(const QString &presetName);

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
    void resetLocalTracks();
    void deletePreset(QAction *action);

    void toggleTransmitingStatus(bool checked);
};

#endif // LOCALTRACKGROUPVIEW_H
