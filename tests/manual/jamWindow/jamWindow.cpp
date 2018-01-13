#include <QApplication>
#include <QDebug>

//#include "../Standalone/MainControllerStandalone.h"
//#include "../Standalone/gui/MainWindowStandalone.h"
//#include "NinjamRoomWindow.h"
//#include "NinjamController.h"
//#include "audio/NinjamTrackNode.h"
//#include "loginserver/LoginService.h"
//#include "NinjamTrackGroupView.h"


//using namespace Controller;


#include <Libs/RtMidi/RtMidi.h>
int main(int argc, char *argv[])
{

    RtMidiIn in;
    in.getPortCount();

    return 0;

//    QApplication::setApplicationName("JamTaba 2");
//    QApplication::setApplicationVersion(APP_VERSION);

//    QApplication app(argc, argv);

//    Configurator* configurator = Configurator::getInstance();
//    if (!configurator->setUp())
//        qCritical() << "JTBConfig->setUp() FAILED !" ;

//    Persistence::Settings settings;
//    settings.load();
//    MainControllerStandalone controller(settings, &app);

//    MainWindowStandalone *mainWindow = new MainWindowStandalone(&controller);
//    controller.setMainWindow(mainWindow);

//    mainWindow->show();
//    mainWindow->setTheme("Rounded");
//    mainWindow->setMinimumSize(920, 700);

//    int maxUsers = 5;
//    Ninjam::Server server("localhost", 2496, 2, maxUsers);

//    for (int i = 0; i < maxUsers; ++i) {
//        Ninjam::User user("test" + QString::number(i));
//        server.addUser(user);
//        server.addUserChannel(Ninjam::UserChannel(user.getFullName(), "channel0", 0));
//        if (rand() % 2 == 0) {
//            server.addUserChannel(Ninjam::UserChannel(user.getFullName(), "channel1", 1));
//        }
//    }

//    controller.connectInNinjamServer(server);

//    // activating the tracks
//    auto ninjamController = controller.getNinjamController();
//    auto tracks = ninjamController->getTrackNodes();
//    for (auto track : tracks) {
//        ninjamController->channelXmitChanged(track->getID(), true);
//    }

//    // setting video frames
//    auto ninjamWindow = mainWindow->getNinjamRomWindow();
//    auto trackGroups = ninjamWindow->getTrackGroups();
//    for (int i =0; i < trackGroups.size(); ++i) {
//        if (i % 2 == 0 ) {
//            int w = qrand() % 100 + 120;
//            int h = qrand() % 50 + 90;
//            QImage image(w, h, QImage::Format_ARGB32);
//            QPainter painter(&image);
//            painter.fillRect(image.rect(), Qt::black);
//            painter.setPen(Qt::white);
//            painter.drawText(image.rect(), Qt::AlignCenter, trackGroups.at(i)->getGroupName());

//            trackGroups.at(i)->updateVideoFrame(image);
//        }
//    }

//    return app.exec();
}
