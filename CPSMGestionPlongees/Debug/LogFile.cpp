#include "LogFile.hpp"

#include <QDate>
#include <QTime>

#include <QFile>
#include <QTextStream>

namespace debug
{

void logFileMsgHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if(!enable_logFile)
        return;

    QString txt;

    static QString fileName{QDate::currentDate().toString("dd-MM-yyyy")+c_extension_log};
    static bool firstCall{true};
    if(firstCall)
    {
        firstCall = false;
        txt += " ---------------------------\n";
    }

    txt += QTime::currentTime().toString("hh:mm:ss - ");

    switch (type)
    {
    case QtDebugMsg:
        txt += QString("%1").arg(msg);
        break;
    case QtInfoMsg:
        txt += QString("Infos : %1").arg(msg);
        break;
    case QtWarningMsg:
        txt += QString("Warning : %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt += QString("Critical : %1").arg(msg);
        break;
    case QtFatalMsg:
        txt += QString("Fatal : %1").arg(msg);
        break;
    }
    txt += "\n";

    QFile f{fileName};
    if(!f.open(QIODevice::Append | QIODevice::WriteOnly))
        throw std::runtime_error{std::string{"Cannot open log file "}+fileName.toStdString().c_str()};
    QTextStream log{&f};
    log << txt;
    f.close();

    (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, msg);
}

}
