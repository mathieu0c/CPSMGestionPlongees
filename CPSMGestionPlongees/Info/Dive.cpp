#include "Dive.h"
#include "../global.hpp"

#include "Info/global.hpp"

#include "Data/Database.hpp"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

namespace info
{

//return the id of the element added. -1 if failed
int addToDB(Dive &dive, QSqlDatabase db, QString table)
{
    if(!db.isOpen())
    {
        QString errMsg{QString{"info::Dive : %1 : database must be opened before being accessed"}.arg(__func__)};

        qCritical() << errMsg;
        throw std::runtime_error(errMsg.toStdString());
        return false;
    }
    QSqlQuery{"BEGIN TRANSACTION;",db};

    QString strQuery{"INSERT INTO %1(date,diveSiteId) VALUES (?,?)"};
    QSqlQuery query{db};
    query.prepare(strQuery.arg(table));
    query.addBindValue(dive.date.toString(global::format_date));
    query.addBindValue(dive.diveSiteId);
    query.exec();

    auto err = query.lastError();
    if(err.type() != QSqlError::ErrorType::NoError)
    {
        QString errStr{QString{"info::Dive : %0 : SQL error : %1 :"}.arg(__func__,err.text())};
        qCritical() << errStr;
        qCritical() << query.lastQuery();
        QSqlQuery{"ROLLBACK;",db};
        return -1;
    }

    int lastId{-1}; //new element id

    query.prepare(QString{"SELECT last_insert_rowid() FROM %1"}.arg(table));
    auto temp{db::querySelect(db,"SELECT last_insert_rowid()",{},{})};
    if(temp.size() > 0)
        lastId = temp[0][0].toInt();


    //--- DiveMembers
    for(const auto& diver : dive.divers)
    {
        if(enableDebug)
            qDebug() << "INSERT INTO : " << global::table_divesMembers;
        query.prepare(QString{"INSERT INTO %1(diveId,diverId,diveType) VALUES (?,?,?)"}.arg(global::table_divesMembers));
        query.addBindValue(lastId);
        query.addBindValue(diver.id);
        query.addBindValue(to_string(diver.type));
        query.exec();

        err = query.lastError();
        if(err.type() != QSqlError::ErrorType::NoError)
        {
            QString errStr{QString{"%0 : SQL error : %1"}.arg(__func__,err.text())};
            qCritical() << errStr;
            QSqlQuery{"ROLLBACK;",db};
            return -1;
        }
    }

    if(lastId != -1)
        QSqlQuery{"COMMIT;",db};
    else
        QSqlQuery{"ROLLBACK;",db};


    return lastId;
}

Dive readDiveFromDB(int id, QSqlDatabase db, QString table)
{
    qDebug() << "##### " << __func__ << " #####";
    static const QString queryStr{"SELECT * FROM %1 WHERE id=?"};
    QSqlQuery query{db};
    query.prepare(queryStr.arg(table));
    query.addBindValue(id);
    query.exec();

    Dive out{};


    if(!query.next())//if nothing was found
    {
        if(enableDebug)
        {
            qDebug() << __func__ << " : No member found with id : " << id;
        }
        return out;
    }

    if(enableDebug)
    {
        qDebug() << "----------- " << __func__ << " -----------";
        qDebug() << "Query column count : " << query.record().count();
    }

    auto err{query.lastError()};
    if(err.type() != QSqlError::ErrorType::NoError)
    {
        QString errStr{QString{"%0 : SQL error : %1"}.arg(__func__,err.text())};
        qCritical() << errStr;
        return out;
    }

    int currentIndex{};

    out.id = query.value(currentIndex++).value<int>();
    out.date = QDate::fromString(query.value(currentIndex++).value<QString>(),global::format_date);
    out.diveSiteId = query.value(currentIndex++).value<int>();

    //count how many diverId there is
    auto diverCount{db::queryCount(db,"SELECT diverId FROM %0 WHERE diveId = ?",{global::table_divesMembers},{out.id})};

    out.divers.reserve(diverCount);

    auto divers{db::querySelect(db,"SELECT diverId,diveType FROM %0 WHERE diveId = ?",{global::table_divesMembers},{out.id})};

    for(const auto& diverLine : divers)
    {
        if(diverLine.size() != 2) //if we didn't get the two columns expected
            throw std::runtime_error{__func__ + std::string{" : Invalid column count : expected 2 and got "}+std::to_string(diverLine.size())};

        Dive::MinimalDiver diver{};
        diver.id = diverLine[0].toInt();
        diver.type = diveTypefrom_string(diverLine[1].toString());
        out.divers.append(diver);
    }

    qDebug() << out;

    return out;
}



QString to_string(DiveType diveType)
{
    switch (diveType)
    {
    case DiveType::exploration:
        return "Explo";
    case DiveType::technical:
        return "Tech";
    default:
        return "-";
    }
}

DiveType diveTypefrom_string(const QString& diveType)
{
    if(diveType == "Explo")
        return DiveType::exploration;
    else if(diveType == "Tech")
        return DiveType::technical;

    return DiveType::undefined;
}

QDebug operator<<(QDebug debug, const Dive& m)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "Dive{\nid : " << m.id << "\n";
    debug.nospace() << "Date : " << m.date.toString(global::format_date) << "\n";
    debug.nospace() << "diveSiteId : " << m.diveSiteId << "\n";
    debug.nospace() << "Divers :" << "\n";
    for(const auto& e : m.divers)
    {
        debug << QString("    Id : %0    |   DiveType : %1").arg(e.id).arg(e.type) << "\n";
    }
    debug.nospace() << "};";

    return debug;
}

} // namespace info
