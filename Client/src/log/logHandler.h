#ifndef LOGHANDLER
#define LOGHANDLER

#include <QMessageLogContext>
#include <QString>

void customLogHandler(QtMsgType, const QMessageLogContext &, const QString &);

#endif // LOGHANDLER

