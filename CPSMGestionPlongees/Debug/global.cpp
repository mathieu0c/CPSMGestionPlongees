#include "Debug/global.hpp"

#include <QSqlError>

namespace debug
{

bool enableLogFile{false};

void debugQuery(const QSqlQuery& query, const QString &fileAndLine)
{
    auto err{query.lastError()};
//    qDebug() << "    --------------- "<<__func__<< " --------------";
    auto qText{query.lastQuery()};
    qDebug() << fileAndLine << ":" << err.text()<<">>>";
    for(const auto& e : query.boundValues())
    {
        qText = qText.replace(qText.indexOf('?'),1,e.toString());
    }
    qDebug() <<"    "<<qText;
//    qDebug() << "°°°°°°°°°°°°° END "<<__func__<< " °°°°°°°°°°°°°";
}

}
