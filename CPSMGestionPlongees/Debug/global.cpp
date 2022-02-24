#include "Debug/global.hpp"


namespace debug
{

bool enableLogFile{false};

void debugQuery(const QSqlQuery& query, const QString &fileAndLine)
{
    qDebug() << "--------------- "<<__func__<< " --------------";
    auto qText{query.lastQuery()};
    qDebug() << fileAndLine;
    for(const auto& e : query.boundValues())
    {
        qText = qText.replace(qText.indexOf('?'),1,e.toString());
    }
    qDebug() << qText;
    qDebug() << "°°°°°°°°°°°°° END "<<__func__<< " °°°°°°°°°°°°°";
}

}
