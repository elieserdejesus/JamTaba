#ifndef NINJAMROOMWINDOW_H
#define NINJAMROOMWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QMessageBox>
#include <QToolButton>

#include "loginserver/LoginService.h"
#include "NinjamTrackGroupView.h"
#include "NinjamPanel.h"
#include "MetronomePanel.h"
#include "intervalProgress/IntervalProgressWindow.h"
#include "chords/ChordProgressionCreationDialog.h"
#include "chat/ChatPanel.h"
#include "ninjam/client/UserChannel.h"

class MainWindow;
class NinjamTrackGroupView;
class NinjamTrackView;

namespace Ui {
    class NinjamRoomWindow;
}

namespace controller {
    class NinjamController;
    class MainController;
}

using ninjam::client::User;
using ninjam::client::UserChannel;
using login::RoomInfo;
using controller::MainController;

class NinjamRoomWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NinjamRoomWindow(MainWindow *parent, const RoomInfo &roomInfo, MainController *mainController);
    ~NinjamRoomWindow();
    void updatePeaks();
    void updateGeoLocations();

    NinjamPanel *getNinjamPanel() const;
    MetronomePanel *getMetronomePanel() const;
    ChordProgressionCreationDialog *getChordProgressionDialog() const;

    void setTracksLayout(TracksLayout newLayout);

    void setTintColor(const QColor &color);

    enum TracksSize
    {
        NARROW,
        WIDE
    };

    void setTracksSize(TracksSize size);

    QString getRoomName() const;

    login::RoomInfo getRoomInfo() const;

    bool metronomeFloatingWindowIsVisible() const;
    void closeMetronomeFloatingWindow();

    void setBpiComboPendingStatus(bool status);
    void setBpmComboPendingStatus(bool status);

    QList<NinjamTrackGroupView *> getTrackGroups() const;

public slots:
    void setChannelXmitStatus(long channelID, bool transmiting);
    void resetBpiComboBox();
    void resetBpmComboBox();
    void showChordProgressionDialog(const ChordProgression &currentProgression);

protected:
    Ui::NinjamRoomWindow *ui;
    MainWindow *mainWindow;
    controller::MainController *mainController;
    NinjamPanel *ninjamPanel; // panel to show interval progress, ninjam BPM/BPI controls, metronome controls, etc
    MetronomePanel *metronomePanel;

    void changeEvent(QEvent *) override;

    void resizeEvent(QResizeEvent *ev) override;

private:
    QMap<QString, NinjamTrackGroupView *> trackGroups;

    login::RoomInfo roomInfo;

    void handleChordProgressionMessage(const User &user, const QString &message);

    NinjamPanel *createNinjamPanel();
    MetronomePanel *createMetronomePanel();

    void setupSignals(controller::NinjamController *ninjamController);

    NinjamTrackView *getTrackViewByID(long trackID);

    TracksLayout tracksLayout;
    TracksSize tracksSize;

    IntervalProgressWindow *metronomeFloatingWindow;
    ChordProgressionCreationDialog *chordProgressionDialog;

    void createLayoutButtons(TracksLayout initialLayout);
    QToolButton *horizontalLayoutButton;
    QToolButton *verticalLayoutButton;
    QToolButton *gridLayoutButton;

    void createTracksSizeButtons(TracksSize lastTracksSize);
    QToolButton *narrowButton;
    QToolButton *wideButton;

    UsersColorsPool *usersColorsPool;

    int calculateEstimatedChunksPerInterval() const;

    void updateTracksSizeButtons(); // enable or disable tracks size buttons

    void updateUserNameLabel();

    void translate();

    void updateBpmBpiLabel();

    void addTrack(NinjamTrackGroupView *track);

    quint8 getGridLayoutMaxCollumns() const;

    void adjustTracksPanelSizePolicy();

private slots:

    // ninjam panel controls
    void setNewBpi(const QString &);
    void setNewBpm(const QString &);
    void handleAccentBeatsComboChange(int);
    void handleMidiSyncChange(bool);
    void handleCustomAccentBeatsChange(const QList<int> &);
    void setNewIntervalShape(int);

    // metronome events
    void setMetronomePanSliderPosition(int value);
    void setMetronomeFaderPosition(int value);
    void toggleMetronomeMuteStatus();
    void toggleMetronomeSoloStatus();
    void showMetronomePreferences();
    void showMetronomeFloatingWindow(bool show);
    void deleteFloatingWindow();

    // video
    void setVideoInterval(const User &user, const QByteArray &encodedVideoData);

    // ninjam controller events
    void addChannel(const User &user, const UserChannel &channel, long channelID);
    void removeChannel(const User &user, const UserChannel &channel, long channelID);
    void changeChannel(const User &user, const UserChannel &channel, long channelID);
    void updateIntervalDownloadingProgressBar(long trackID);
    void hideIntervalDownloadingProgressBar(long trackID);

    void handleBpiChanges();
    void handleBpmChanges();

    void showServerLicence();

    void toggleTracksLayoutOrientation(QAbstractButton *buttonClicked); // horizontal or vertical
    void toggleTracksSize(QAbstractButton *buttonClicked); // narrow or wide

    void setEstimatatedChunksPerIntervalInAllTracks();

    void reAddTrackGroups();

    void updateStylesheet();
};

inline ChordProgressionCreationDialog *NinjamRoomWindow::getChordProgressionDialog() const
{
    return chordProgressionDialog;
}

inline QList<NinjamTrackGroupView *> NinjamRoomWindow::getTrackGroups() const
{
    return trackGroups.values();
}

inline MetronomePanel *NinjamRoomWindow::getMetronomePanel() const
{
    return metronomePanel;
}

inline NinjamPanel *NinjamRoomWindow::getNinjamPanel() const
{
    return ninjamPanel;
}

inline QString NinjamRoomWindow::getRoomName() const
{
    return roomInfo.getName();
}

inline login::RoomInfo NinjamRoomWindow::getRoomInfo() const
{
    return roomInfo;
}

#endif // NINJAMROOMWINDOW_H
