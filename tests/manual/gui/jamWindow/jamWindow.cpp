#include <QApplication>
#include <QDebug>
#include <QByteArray>

#include "../Standalone/MainControllerStandalone.h"
#include "../Standalone/gui/MainWindowStandalone.h"
#include "NinjamRoomWindow.h"
#include "NinjamController.h"
#include "audio/NinjamTrackNode.h"
#include "loginserver/LoginService.h"
#include "NinjamTrackGroupView.h"
#include <QPainter>


using namespace Controller;

int main(int argc, char *argv[])
{

    QApplication::setApplicationName("JamTaba 2");
    QApplication::setApplicationVersion(APP_VERSION);

    QApplication app(argc, argv);

    Configurator* configurator = Configurator::getInstance();
    if (!configurator->setUp())
        qCritical() << "JTBConfig->setUp() FAILED !" ;

    Persistence::Settings settings;
    settings.load();
    MainControllerStandalone controller(settings, &app);

    MainWindowStandalone *mainWindow = new MainWindowStandalone(&controller);
    controller.setMainWindow(mainWindow);

    mainWindow->show();
    mainWindow->setTheme("volcano_nm");
    mainWindow->setMinimumSize(920, 700);

    int maxUsers = 8;
    Ninjam::Server server("localhost", 2496, 2, maxUsers);
    for (int i = 0; i < maxUsers; ++i) {
        Ninjam::User user("teste" + QString::number(i));
        server.addUser(user);
        server.addUserChannel(Ninjam::UserChannel(user.getFullName(), "channel0", 0));
    }

    controller.connectInNinjamServer(server);

    // activating the tracks
    auto ninjamController = controller.getNinjamController();
    auto tracks = ninjamController->getTrackNodes();
    for (auto track : tracks) {
        ninjamController->channelXmitChanged(track->getID(), true);
    }

    // setting video frames
    auto ninjamWindow = mainWindow->getNinjamRomWindow();
    auto trackGroups = ninjamWindow->getTrackGroups();
    for (auto group : trackGroups) {
        int w = qrand() % 100 + 120;
        int h = qrand() % 50 + 90;
        QImage image(w, h, QImage::Format_ARGB32);
        QPainter painter(&image);
        painter.fillRect(image.rect(), Qt::black);
        painter.setPen(Qt::white);
        painter.drawText(image.rect(), Qt::AlignCenter, group->getGroupName());

        group->updateVideoFrame(image);
    }

    return app.exec();
}
