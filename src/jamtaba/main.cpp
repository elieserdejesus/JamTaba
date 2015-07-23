#include "gui/MainFrame.h"
#include "MainController.h"
#include "JamtabaFactory.h"
#include "persistence/Settings.h"
#include <QTextCodec>
#include <QLoggingCategory>
#include <QDir>
#include <QFile>

#include "../audio/vorbis/VorbisEncoder.h"
#include "../audio/core/AudioNode.h"
#include "../audio/Resampler.h"
#include "../audio/SamplesBufferResampler.h"
#include "../audio/core/SamplesBuffer.h"

void customLogHandler(QtMsgType, const QMessageLogContext &, const QString &);

int main(int argc, char* args[] ){
/*
    SamplesBufferResampler resampler;

    Audio::SamplesBuffer in(1, 64);
    in.zero();
    for (int x = 0; x < in.getFrameLenght(); ++x) {
        in.set(0, x, (float)x/in.getFrameLenght());
    }


    //se eu passo a flag de final na hora errada vem muitas amostras a mais, 10x mais
    int total = 0;
    int resampled = 0;
    for (int x = 0; x < 10; ++x) {
        Audio::SamplesBuffer resampledBuffer = resampler.resample(in, 1, in.getFrameLenght()==1, 128 , 2);
        resampled = resampledBuffer.getFrameLenght();
        total += resampled;
        qWarning() << "new size: " << resampledBuffer.getFrameLenght();

//        for (int x = 0; x < resampledBuffer.getFrameLenght(); ++x) {
//            qWarning() << resampledBuffer.get(0, x);
//        }
        if(x == 0){
            in.setFrameLenght(1);
        }
        else{
            in.setFrameLenght(0);
        }
    }

    qWarning() << "Total: " << total;
    return 0;

    */



    QApplication::setApplicationName("Jamtaba 2");

    qputenv("QT_LOGGING_CONF", ":/qtlogging.ini");//log cconfigurations is in resources at moment

    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    qInstallMessageHandler(customLogHandler);

    JamtabaFactory* factory = new ReleaseFactory();
    Persistence::Settings settings;//read from file in constructor
    settings.load();

    Controller::MainController mainController(factory, settings, argc, args);//MainController extends QApplication

    MainFrame mainFrame(&mainController);
    mainFrame.show();

    delete factory;
    int returnCode = mainController.exec();
    mainController.saveLastUserSettings(mainFrame.getInputsSettings());
    return returnCode;

 }
//++++++++++++++++++++++++++++++++++

void customLogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QString stringMsg;
    QString fullFileName(context.file);
    const char* file;
    int lastPathSeparatorIndex = fullFileName.lastIndexOf(QDir::separator());
    if(lastPathSeparatorIndex){
        file = fullFileName.right(fullFileName.size() - lastPathSeparatorIndex - 1).toStdString().c_str();
    }
    else{
        file = fullFileName.toStdString().c_str();
    }

    switch (type) {
    case QtDebugMsg:
        stringMsg = QString::asprintf("\nDEBUG: %s [%s: %u]", localMsg.constData(), file , context.line);
        break;
    case QtWarningMsg:
        stringMsg = QString::asprintf("\n\nWARNING: %s (%s) [%s:%u]\n", localMsg.constData(), context.function, file, context.line);
        break;
    case QtCriticalMsg:
        stringMsg = QString::asprintf("\n\nCRITICAL: %s (%s) [%s:%u]\n\n", localMsg.constData(), context.function, file, context.line);
        break;
    case QtFatalMsg:
        stringMsg = QString::asprintf("\n\nFATAL: %s (%s) [%s:%u]\n\n", localMsg.constData(), context.function, file, context.line);
        //abort();
        break;
    case QtInfoMsg:
        stringMsg = QString::asprintf("\n\nINFO: %s [%s:%u]\n\n", localMsg.constData(), file, context.line);
    }
    fprintf(stderr, stringMsg.toStdString().c_str());//write log message to console
    fflush(stderr);

    if(type != QtDebugMsg){//write the critical messages to log file
        QFile outFile("log.txt");
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << stringMsg << "\n\r";
    }

    if(type == QtFatalMsg){
        abort();
    }
}

