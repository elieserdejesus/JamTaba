#include "gui/MainFrame.h"
#include "MainController.h"
#include "JamtabaFactory.h"
#include <QTextCodec>

void customLogHandler(QtMsgType, const QMessageLogContext &, const QString &);

#include "audio/core/SamplesBuffer.h"
#include "audio/vorbis/VorbisEncoder.h"
#include <QFile>
#include "audio/core/AudioNode.h"

int main(int argc, char* args[] ){

//    int sampleRate = 44100;
//    Audio::OscillatorAudioNode osc(400, sampleRate);
//    Audio::SamplesBuffer oscSamples(2, sampleRate * 5);
//    Audio::SamplesBuffer in(2, 0);
//    osc.processReplacing(in, oscSamples);
//    //int encoded = 0;

//    for (int x = 0; x < 5; ++x) {
//        VorbisEncoder encoder(2, sampleRate);
//        Audio::SamplesBuffer temp(2, sampleRate);
//        temp.set(oscSamples, x * sampleRate, sampleRate, 0);
//        QByteArray encodedData = encoder.encode(temp);
//        encodedData.append(encoder.finishIntervalEncoding());

//        QString fileName = "test" + QString::number(x) + ".ogg";
//        QFile file(fileName);
//        file.open(QIODevice::WriteOnly);
//        file.write(encodedData.data(), encodedData.size());
//    }

//    return 0;

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    qInstallMessageHandler(customLogHandler);

    JamtabaFactory* factory = new ReleaseFactory();

    Controller::MainController mainController(factory, argc, args);//MainController extends QApplication
    MainFrame mainFrame(&mainController);
    mainFrame.show();

    delete factory;
    return mainController.exec();

 }
//++++++++++++++++++++++++++++++++++

void customLogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "DEBUG: %s [%s: %u]\n", localMsg.constData(), context.file , context.line);
        break;
    case QtWarningMsg:
        fprintf(stderr, "\n\nWARNING: %s (%s) [%s:%u]\n\n", localMsg.constData(), context.function, context.file, context.line);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "\n\nCRITICAL: %s (%s) [%s:%u]\n\n", localMsg.constData(), context.function, context.file, context.line);
        break;
    case QtFatalMsg:
        fprintf(stderr, "\n\nFATAL: %s (%s) [%s:%u]\n\n", localMsg.constData(), context.function, context.file, context.line);
        abort();
    }
    fflush(stderr);
}

