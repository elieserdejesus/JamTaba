#include "gui/mainframe.h"
#include "MainController.h"
#include "JamtabaFactory.h"
#include <QApplication>
#include <QLoggingCategory>
#include <QDir>

using namespace Controller;
using namespace Persistence;

void customLogHandler(QtMsgType, const QMessageLogContext &, const QString &);

class ThemeEditorFactory : public JamtabaFactory{

public:
    Login::LoginService* createLoginService(QObject *parent){
        Q_UNUSED(parent);
        return nullptr;
    }
};
//++++++++++++++++++++++++++++++++++
class NullSettings : public Settings{
public:
    NullSettings(){

    }

};

//++++++++++++++++++++++++++++++++++
class ThemeEditorMainController : public Controller::MainController{
public:
    ThemeEditorMainController(int &argc, char** args)
        :MainController(new ThemeEditorFactory(), NullSettings(), argc, args){

    }

};

//+++++++++++++++++++++++++++++++++++++++++++++++

int main(int argc, char* args[] ){

    QApplication::setApplicationName("Jamtaba 2 Theme Editor");
    QApplication::setApplicationVersion(APP_VERSION);

    qputenv("QT_LOGGING_CONF", ":/qtlogging.ini");//log configurations is in resources at moment

    qInstallMessageHandler(customLogHandler);

    ThemeEditorMainController mainController(argc, args);//MainController extends QApplication

    MainFrame mainFrame(&mainController);
    mainFrame.show();

    int returnCode = mainController.exec();
    //mainController.saveLastUserSettings(mainFrame.getInputsSettings());

    return returnCode;

 }

//++++++++++++++++++++++++++++++++++

void customLogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QString stringMsg;
    QString fullFileName(context.file);
    QString file;
    int lastPathSeparatorIndex = fullFileName.lastIndexOf(QDir::separator());
    if(lastPathSeparatorIndex){
        file = fullFileName.right(fullFileName.size() - lastPathSeparatorIndex - 1);//.toStdString().c_str();
    }
    else{
        file = fullFileName;//.toStdString().c_str();
    }

    QTextStream stream(&stringMsg);
    switch (type) {
    case QtDebugMsg:
        stream << "\nDEBUG:" << localMsg.constData() << " "  << " in " << file << " " << context.line << endl;
        break;
    case QtWarningMsg:
        stream << "\n\nWARNING: " << localMsg.constData() <<  context.function <<  " " << file << context.line << endl;
        break;
    case QtCriticalMsg:
        stream << "\n\nCRITICAL:" << localMsg.constData() <<  context.function << " " << file << context.line << endl <<endl;
        break;
    case QtFatalMsg:
        stream << "\n\nFATAL:" << localMsg.constData() << context.function << file << context.line << endl << endl;
        //abort();
        break;
    default:
       stream << "\n\nINFO:" << localMsg.constData() << file << context.line << endl << endl;
    }
    QTextStream(stdout) << stringMsg << endl;

    if(type == QtFatalMsg){
        abort();
    }
}


