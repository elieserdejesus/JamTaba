#ifndef NINJAMROOMWINDOW_H
#define NINJAMROOMWINDOW_H

#include <QWidget>
#include <QTimer>
#include "ninjam/UserChannel.h"
#include "loginserver/LoginService.h"
#include "chat/ChatPanel.h"
#include "NinjamTrackGroupView.h"
#include <QMessageBox>
#include "NinjamPanel.h"
#include "MetronomePanel.h"
#include "intervalProgress/IntervalProgressWindow.h"

class MainWindow;
class NinjamTrackGroupView;
class NinjamTrackView;
class QToolButton;

namespace Ui {
class NinjamRoomWindow;
}

namespace Controller {
class NinjamController;
class MainController;
}

class NinjamRoomWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NinjamRoomWindow(MainWindow *parent, const Login::RoomInfo &roomInfo,
                              Controller::MainController *mainController);
    ~NinjamRoomWindow();
    void updatePeaks();
    void updateGeoLocations();

    NinjamPanel *getNinjamPanel() const;
    MetronomePanel *getMetronomePanel() const;

    void setTracksLayout(TracksLayout newLayout);

    enum TracksSize
    {
        NARROW,
        WIDE
    };

    void setTracksSize(TracksSize size);

    QString getRoomName() const;

    Login::RoomInfo getRoomInfo() const;

    bool metronomeFloatingWindowIsVisible() const;
    void closeMetronomeFloatingWindow();

    void setBpiComboPendingStatus(bool status);
    void setBpmComboPendingStatus(bool status);

    QList<NinjamTrackGroupView *> getTrackGroups() const;

public slots:
    void setChannelXmitStatus(long channelID, bool transmiting);
    void resetBpiComboBox();
    void resetBpmComboBox();

protected:
    Ui::NinjamRoomWindow *ui;
    MainWindow *mainWindow;
    Controller::MainController *mainController;
    NinjamPanel *ninjamPanel; // panel to show interval progress, ninjam BPM/BPI controls, metronome controls, etc
    MetronomePanel *metronomePanel;

    void changeEvent(QEvent *) override;

    void resizeEvent(QResizeEvent *ev) override;

private:
    QMap<QString, NinjamTrackGroupView *> trackGroups;

    Login::RoomInfo roomInfo;

    void handleChordProgressionMessage(const Ninjam::User &user, const QString &message);

    NinjamPanel *createNinjamPanel();
    MetronomePanel *createMetronomePanel();

    void setupSignals(Controller::NinjamController *ninjamController);

    NinjamTrackView *getTrackViewByID(long trackID);

    TracksLayout tracksLayout;
    TracksSize tracksSize;

    IntervalProgressWindow *metronomeFloatingWindow;

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
    void setVideoInterval(const Ninjam::User &user, const QByteArray &encodedVideoData);

    // ninjam controller events
    void addChannel(const Ninjam::User &user, const Ninjam::UserChannel &channel, long channelID);
    void removeChannel(const Ninjam::User &user, const Ninjam::UserChannel &channel, long channelID);
    void changeChannelName(const Ninjam::User &user, const Ninjam::UserChannel &channel, long channelID);
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

inline Login::RoomInfo NinjamRoomWindow::getRoomInfo() const
{
    return roomInfo;
}

#endif // NINJAMROOMWINDOW_H
