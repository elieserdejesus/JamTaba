#ifndef LOCALTRACKGROUPVIEW_H
#define LOCALTRACKGROUPVIEW_H

#include "TrackGroupView.h"
#include "LocalTrackView.h"

class MainWindow;
class QPushButton;
class BlinkableButton;
class InstrumentsButton;

class LocalTrackGroupView : public TrackGroupView
{
    Q_OBJECT

public:
    LocalTrackGroupView(int index, MainWindow *mainFrame);

    virtual ~LocalTrackGroupView();

    LocalTrackView *addTrackView(long trackID) override;

    int getChannelIndex() const;

    virtual void setPeakMeterMode(bool peakMeterOnly);
    virtual void togglePeakMeterOnlyMode();
    bool isShowingPeakMeterOnly() const;

    void detachMainControllerInSubchannels();
    void closePluginsWindows();

    void setToNarrow();
    void setToWide();

    void setPreparingStatus(bool preparing); // preparing to transmit
    bool isPreparingToTransmit() const;

    void resetTracks();

    int getSubchannelInternalIndex(uint subchannelTrackID) const;

    BlinkableButton *getXmitButton() const;

    QSize sizeHint() const override; // fixing #962

    QString getChannelGroupName() const;

    void setInstrumentIcon(int instrumentIndex);
    int getInstrumentIcon() const;

protected:

    LocalTrackView *createTrackView(long trackID) override;

    void translateUi() override;

    virtual void populateMenu(QMenu &menu);

    static const int MAX_SUB_CHANNELS = 2;

    MainWindow *mainFrame;

private:
    QPushButton *toolButton;
    BlinkableButton *xmitButton;
    InstrumentsButton *instrumentsButton;

    bool preparingToTransmit;
    bool usingSmallSpacingInLayouts;

    int index;

    bool peakMeterOnly;

    QPushButton *createToolButton();
    BlinkableButton *createXmitButton();

    QMenu* createPresetsLoadingSubMenu();
    QMenu* createPresetsDeletingSubMenu();
    void createPresetsActions(QMenu &menu);
    void createChannelsActions(QMenu &menu);

    InstrumentsButton *createInstrumentsButton();

    void updateXmitButtonText();

    static QString getStripedPresetName(const QString &presetName);

signals:
    void instrumentIconChanged();
    void trackRemoved();
    void trackAdded();
    void presetLoaded();
    void presetSaved();

protected slots:
    virtual void removeSubchannel();

private slots:
    void showMenu();

    void addSubChannel();
    void addChannel();

    void highlightHoveredSubchannel();
    void highlightHoveredChannel();

    void removeChannel();

    void loadPreset(QAction *action);
    void savePreset();
    void resetLocalTracks();
    void deletePreset(QAction *action);

    void toggleTransmitingStatus(bool checked);
};

inline BlinkableButton *LocalTrackGroupView::getXmitButton() const
{
    return xmitButton;
}

inline bool LocalTrackGroupView::isPreparingToTransmit() const
{
    return preparingToTransmit;
}

inline bool LocalTrackGroupView::isShowingPeakMeterOnly() const
{
    return peakMeterOnly;
}

inline int LocalTrackGroupView::getChannelIndex() const
{
    return index;
}

#endif // LOCALTRACKGROUPVIEW_H
