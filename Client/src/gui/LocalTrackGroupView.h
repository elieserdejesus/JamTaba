#ifndef LOCALTRACKGROUPVIEW_H
#define LOCALTRACKGROUPVIEW_H

#include "TrackGroupView.h"

class LocalTrackView;

class QPushButton;
class MainWindow;

namespace Ui {
class LocalTrackGroupView;
//class PresetMenu;
}


class LocalTrackGroupView : public TrackGroupView
{
    Q_OBJECT
public:
    LocalTrackGroupView(int index, MainWindow* mainFrame);
    ~LocalTrackGroupView();
    void refreshInputSelectionName(int inputTrackIndex);
    void refreshInputSelectionNames();
    QList<LocalTrackView*> getTracks() const;
    virtual void addTrackView(BaseTrackView *trackView);
    inline int getChannelIndex() const{return index;}

    virtual void setPeakMeterMode(bool peakMeterOnly);
    virtual void togglePeakMeterOnlyMode();
    bool isShowingPeakMeterOnly() const{return peakMeterOnly;}

    void detachMainControllerInSubchannels();
    void closePluginsWindows();
    //QSize sizeHint() const;

    void setToNarrow();
    void setToWide();

    void setPreparingStatus(bool preparing);//preparing to transmit
    inline bool isPreparingToTransmit() const{return preparingToTransmit;}

signals:
    void nameChanged();
    void trackRemoved();
    void trackAdded();
    void presetLoaded();
    void presetSaved();

private:
    QPushButton* toolButton;
    QPushButton* xmitButton;
    bool preparingToTransmit;

    static const int MAX_SUB_CHANNELS = 2;
    int index;
    MainWindow* mainFrame;
    bool eventFilter(QObject *target, QEvent *event);

    bool peakMeterOnly;

    QPushButton* createToolButton();
    QPushButton* createXmitButton();

private slots:
    void on_toolButtonClicked();

    void on_addSubChannelClicked();
    void on_addChannelClicked();

    void on_removeSubchannelHovered();
    void on_removeChannelHovered();

    void on_removeSubChannelClicked();
    void on_removeChannelClicked();


    //PRESETS
    void on_LoadPresetClicked(QAction *a);
    void on_SavePresetClicked();
    void on_ResetPresetClicked();
    void on_presetMenuActionClicked();
    void on_RemovePresetClicked();


    //xmit
    void on_xmitButtonClicked(bool checked);



};


#endif // LOCALTRACKGROUPVIEW_H
