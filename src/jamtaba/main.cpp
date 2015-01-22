#include "gui/mainframe.h"
#include <QtWidgets/QApplication>
#include <QDebug>
#include <stdexcept>
#include <QFile>
#include <QFileInfo>
#include <QResource>
#include "network/loginserver/DefaultLoginService.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DefaultLoginService loginService;
    NatMap natMap;
    loginService.connect("elieser", 0,"nome do canal", natMap, 1, "windows", 44100);

    QFile styleFile( ":/style/jamtaba.css" );
    if(!styleFile.open( QFile::ReadOnly )){
        qFatal("não carregou estilo!");
    }

    // Apply the loaded stylesheet
    QString style( styleFile.readAll() );
    app.setStyleSheet( style );

     try{
        MainFrame w;
        w.show();
        return app.exec();
    }
    catch(const std::runtime_error& ex){
        qDebug() << ex.what();
    }
    return 0;

}
