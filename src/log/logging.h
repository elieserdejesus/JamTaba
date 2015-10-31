#ifndef LOGHANDLER
#define LOGHANDLER

#include <QMessageLogContext>
#include <QLoggingCategory>
#include <QString>

//good read about Qt logging: https://blog.qt.io/blog/2014/03/11/qt-weekly-1-categorized-logging/

Q_DECLARE_LOGGING_CATEGORY(jtCore)
Q_DECLARE_LOGGING_CATEGORY(jtLoginService)
Q_DECLARE_LOGGING_CATEGORY(jtNinjamCore)
Q_DECLARE_LOGGING_CATEGORY(jtNinjamProtocol)
Q_DECLARE_LOGGING_CATEGORY(jtNinjamGUI)
Q_DECLARE_LOGGING_CATEGORY(jtGUI)
Q_DECLARE_LOGGING_CATEGORY(jtNinjamVorbisEncoder)
Q_DECLARE_LOGGING_CATEGORY(jtNinjamVorbisDecoder)
Q_DECLARE_LOGGING_CATEGORY(jtNinjamRoomStreamer)
Q_DECLARE_LOGGING_CATEGORY(jtJamRecorder)
Q_DECLARE_LOGGING_CATEGORY(jtIpToLocation)
Q_DECLARE_LOGGING_CATEGORY(jtStandaloneVstHost)
Q_DECLARE_LOGGING_CATEGORY(jtStandaloneVstPlugin)
Q_DECLARE_LOGGING_CATEGORY(jtVstPlugin)
Q_DECLARE_LOGGING_CATEGORY(jtAudio)
Q_DECLARE_LOGGING_CATEGORY(jtMidi)



void jamtabaLogHandler(QtMsgType, const QMessageLogContext &, const QString &);

#endif // LOGHANDLER

