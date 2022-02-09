#include "DBApi/Database.hpp"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

#include <QSqlRecord>
#include <QSqlResult>

#include <QStringView>
#include <QString>
#include <QVariant>
#include <QVector>

#include <QDebug>

namespace db
{

bool openLocal(const QString& fileName,const QString& connectionName)
{
    QSqlDatabase db;
    if(connectionName.isEmpty())
        db = QSqlDatabase::addDatabase("QSQLITE");
    else
        db = QSqlDatabase::addDatabase("QSQLITE",connectionName);
    db.setHostName("localhost");
    db.setDatabaseName(fileName+c_extension_DB);
    if(!db.open())
    {
        if(enableDebug)
            qDebug() << "Cannot open db : " << db.lastError();
        return false;
    }

    return true;
}

//template<typename T>
int countField(QSqlDatabase& db,const QString& table,const QString& field,QVariant value)
{
    QString queryStr{"SELECT COUNT(*) FROM %0 WHERE %1=?"};
    QSqlQuery query{db};
    query.prepare(queryStr.arg(table,field));
    query.addBindValue(value);
    query.exec();

    if(!query.next())//if nothing was found
    {
        if(enableDebug)
        {
            qDebug() << __func__ << " : Cannot satisfy request : <" << query.last()<<">";
        }
    }

    auto err{query.lastError()};
    if(err.type() != QSqlError::ErrorType::NoError)//if there is an error
    {
        QString errStr{QString{"%0 : SQL error : %1"}.arg(__func__,err.text())};
        qCritical() << errStr;
        return -1;
    }

    if(enableDebug)
        qDebug() << query.value(0);
    return 0;
}

QVector<QVector<QVariant>> querySelect(QSqlDatabase db,QString request,const QStringList& argList,const QVector<QVariant>& valList)
{
    QSqlQuery query{db};

    for(const auto& e : argList)//match argument list
    {
        request = request.arg(e);
    }

    query.prepare(request);//prepare sql query

    for(const auto& e : valList) //bind all values (this way add some injection protection)
    {
        query.addBindValue(e);
    }

    query.exec();


    QVector<QVector<QVariant>> out{};
    int lineSize{query.record().count()};

    while(query.next())//while we find something correspondant
    {
        out.resize(out.size()+1);//resize the output vector

        out.back().resize(lineSize);//resize line to match column count

        for(int i{}; i < lineSize;++i)
        {
            out.back()[i] = query.value(i);
        }
    }

    auto err{query.lastError()};
    if(err.type() != QSqlError::ErrorType::NoError)//if there was an error
    {
        QString errStr{QString{"%0 : SQL error : %1 : "}.arg(__func__,err.text())};
        qCritical() << errStr;
        qCritical() << query.lastQuery();
        return {};
    }

    return out;
}

int queryCount(QSqlDatabase& db,QString request,const QStringList& argList,const QVector<QVariant>& valList)
{
    QSqlQuery query{db};

    for(const auto& e : argList)//match argument list
    {
        request = request.arg(e);
    }


    QRegularExpression re("(SELECT )(.*)( FROM.*)");
    auto matchs{re.globalMatch(request)};

    QStringList groupList{};
    while(matchs.hasNext())
    {
        auto match{matchs.next()};
        groupList << match.captured(1);
        groupList << match.captured(2);
        groupList << match.captured(3);
    }
    if(groupList.size() != 3)//not matched expected occurencies
        throw std::runtime_error{"Invalid match count"};

    request = groupList[0] + "COUNT(" + groupList[1] + ")" + groupList[2];

    query.prepare(request);//prepare sql query

    for(const auto& e : valList) //bind all values (this way add some injection protection)
    {
        query.addBindValue(e);
    }

    query.exec();


    if(!query.next())
    {
        qCritical() << __func__ << " : No valid result";
        return -1;
    }
    int out{query.value(0).toInt()};

//    while(query.next())//while we find something correspondant
//    {
//        out += 1;
//    }

    auto err{query.lastError()};
    if(err.type() != QSqlError::ErrorType::NoError)//if there was an error
    {
        QString errStr{QString{"%0 : SQL error : %1 : "}.arg(__func__,err.text())};
        qCritical() << errStr;
        qCritical() << query.lastQuery();
        return {};
    }

    return out;
}

bool queryExist(QSqlDatabase& db,QString request,const QStringList& argList,const QVector<QVariant>& valList)
{
    QSqlQuery query{db};

    for(const auto& e : argList)//match argument list
    {
        request = request.arg(e);
    }

    query.prepare(request);//prepare sql query

    for(const auto& e : valList) //bind all values (this way add some injection protection)
    {
        query.addBindValue(e);
    }

    query.exec();

    if(query.next())//if the query returned something
        return true;

    //else
    return false;
}

}//namespace dta