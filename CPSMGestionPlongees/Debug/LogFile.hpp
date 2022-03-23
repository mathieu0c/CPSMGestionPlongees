#ifndef DEBUG_LOGFILE_HPP
#define DEBUG_LOGFILE_HPP

#include <QString>
#include <QtMessageHandler>
#include <QMessageLogContext>

namespace debug
{

static bool enable_logFile{true};


static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(0);

void logFileMsgHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

inline void startLog(){qInstallMessageHandler(logFileMsgHandler);}
}

#endif // LOGFILE_HPP
