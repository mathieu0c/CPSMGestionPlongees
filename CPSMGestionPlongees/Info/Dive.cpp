#include "Dive.h"
#include "../global.hpp"

#include "Info/global.hpp"

#include "Data/Database.hpp"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include <algorithm>

namespace info
{

bool removeAllFromDB(const Dive& dive,QSqlDatabase db, const QString& table)
{
    QSqlQuery{"BEGIN TRANSACTION;",db};

    //------------------------- Delete from DivesMembers table
    QString idList{};
    for(const auto& e : dive.divers)
    {
        idList += QString::number(e.id)+',';
    }
    idList.chop(1);//remove last ','

    static const QString queryStr{"DELETE FROM %0 WHERE diveId=? AND diverId IN (%1)"};
    QSqlQuery query{db};
    query.prepare(queryStr.arg(global::table_divesMembers,std::move(idList)));
    query.addBindValue(dive.id);
    query.exec();

    auto err{query.lastError()};
    if(err.type() != QSqlError::ErrorType::NoError)
    {
        QString errStr{QString{"%0:%1:%2 : SQL error : %3"}.arg(__FILE__,_LINE_,__func__,err.text())};
        qCritical() << errStr;
        QSqlQuery{"ROLLBACK;",db};
        return false;
    }

    //------------------------- Delete from Dives table

    static const QString queryStr2{"DELETE FROM %0 WHERE id=?"};
    QSqlQuery query2{db};
    query2.prepare(queryStr2.arg(table));
    query2.addBindValue(dive.id);
    query2.exec();

    auto err2{query2.lastError()};
    if(err2.type() != QSqlError::ErrorType::NoError)
    {
        QString errStr{QString{"%0:%1:%2 : SQL error : %3"}.arg(__FILE__,_LINE_,__func__,err.text())};
        qCritical() << errStr;
        QSqlQuery{"ROLLBACK;",db};
        return false;
    }

    QSqlQuery{"COMMIT;",db};

    return true;
}

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

    QString strQuery{"INSERT INTO %1(date,time,diveSiteId) VALUES (?,?,?)"};
    QSqlQuery query{db};
    query.prepare(strQuery.arg(table));
    query.addBindValue(dive.date.toString(global::format_date));
    query.addBindValue(dive.time.toString(global::format_time));
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
    dive.id = lastId;


    //--- DiveMembers
    for(const auto& diver : dive.divers)
    {
        if(enableDebug)
            qDebug() << "INSERT INTO : " << global::table_divesMembers;
        auto minDiverSuccess{addToDB(diver,dive.id,db,global::table_divesMembers)};

        if(!minDiverSuccess)
        {
            QString errStr{QString{"%0 : SQL error : %1"}.arg(__func__,"Minimal Dive addition")};
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

bool updateDB(Dive& dive,QSqlDatabase db,QString table,bool checkExistence)
{
    if(checkExistence)
    {
        if(!exists(dive,db,table))//if the dive doesn't exist
            return false;
    }

    QSqlQuery{"BEGIN TRANSACTION;",db};

    static const QString queryStr{"UPDATE %1 SET "
                                  "date = ?,"
                                  "time = ?,"
                                  "diveSiteId = ? "
                                  "WHERE %1.id = ?"};

    QSqlQuery query{db};
    query.prepare(queryStr.arg(table));
    query.addBindValue(dive.date.toString(global::format_date));
    query.addBindValue(dive.time.toString(global::format_time));
    query.addBindValue(dive.diveSiteId);
    query.addBindValue(dive.id);
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

    for(const auto& diver : dive.divers)
    {
        auto minDiverSuccess{storeInDB(diver,dive.id,db,global::table_divesMembers)};

        if(!minDiverSuccess)
        {
            QString errStr{QString{"%0 : SQL error : %1"}.arg(__func__,"Minimal Dive addition")};
            qCritical() << errStr;
            QSqlQuery{"ROLLBACK;",db};
            return false;
        }
    }

    if(!removeFromDBNotIn(dive.divers,dive.id,db,global::table_divesMembers))
    {
        QString errStr{QString{"%0:%1:%2 : SQL error : %3"}.arg(__FILE__,_LINE_,__func__,"Minimal Dive addition")};
        qCritical() << errStr;
        QSqlQuery{"ROLLBACK;",db};
        return false;
    }

    QSqlQuery{"COMMIT;",db};

    return true;
}

Dive readDiveFromDB(int id, QSqlDatabase db, QString table)
{
//    qDebug() << "##### " << __func__ << " #####";
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
    out.time = QTime::fromString(query.value(currentIndex++).value<QString>(),global::format_time);
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

//    qDebug() << out;

    return out;
}

int exists(const Dive& a,QSqlDatabase db,const QString& table)
{
    auto temp{db::querySelect(db,"SELECT id FROM %1 WHERE %1.id = ?",{table},{a.id})};

    if(temp.size() > 0)
    {
        return temp[0][0].toInt();
    }
    return -1;
}

void removeDiversFromDive(Dive& dive,QVector<int> idList)
{
    dive.divers.erase(std::remove_if(dive.divers.begin(),dive.divers.end(),[&](Dive::MinimalDiver& diver){
        return idList.contains(diver.id);
    }),dive.divers.end());
}

DiveType getDiveTypeForDiver(const Dive& dive,int diverId)
{
    auto itDiver{std::find_if(dive.divers.begin(),dive.divers.end(),[&](const Dive::MinimalDiver& e){
            return e.id == diverId;
        })};
    if(itDiver == dive.divers.end())
    {
        return DiveType::undefined;
    }

    return (*itDiver).type;
}

bool setDiveTypeForDiver(Dive& dive,int diverId,DiveType type)
{
    auto itDiver{std::find_if(dive.divers.begin(),dive.divers.end(),[&](const Dive::MinimalDiver& e){
            return e.id == diverId;
        })};
    if(itDiver == dive.divers.end())
    {
        return false;
    }
    (*itDiver).type = type;

    return true;
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
    debug.nospace() << "Time : " << m.time.toString(global::format_time) << "\n";
    debug.nospace() << "diveSiteId : " << m.diveSiteId << "\n";
    debug.nospace() << "Divers :" << "\n";
    for(const auto& e : m.divers)
    {
        debug << QString("    Id : %0    |   DiveType : %1").arg(e.id).arg(e.type) << "\n";
    }
    debug.nospace() << "};";

    return debug;
}


//-------------------------------------------------------------------------------------
//------------------------  MinimalDiver funcs  ---------------------------------------
//-------------------------------------------------------------------------------------

QDebug operator<<(QDebug debug, const Dive::MinimalDiver& d)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "MinimalDiver{\nid : " << d.id << "\n";
    debug.nospace() << "Type : " << to_string(d.type) << "\n";
    debug.nospace() << "};";

    return debug;
}

bool addToDB(const Dive::MinimalDiver& diver, int diveId, QSqlDatabase db, QString table)
{
//    if(!db.isOpen())
//    {
//        QString errMsg{QString{"info::Dive::addToDB(MinimalDiver) : %1 : database must be opened before being accessed"}.arg(__func__)};

//        qCritical() << errMsg;
//        throw std::runtime_error(errMsg.toStdString());
//        return false;
//    }%0:%1:%2

    QString strQuery{"INSERT INTO %1(diveId,diverId,diveType) VALUES (?,?,?)"};
    QSqlQuery query{db};
    query.prepare(strQuery.arg(table));
    query.addBindValue(diveId);
    query.addBindValue(diver.id);
    query.addBindValue(to_string(diver.type));
    query.exec();

    auto err = query.lastError();
    if(err.type() != QSqlError::ErrorType::NoError)
    {
        qDebug() << diver;
        QString errStr{QString{"%0:%1:%2 : SQL error : %3 :"}.arg(__FILE__,__func__,QString::number(__LINE__),err.text())};
        qCritical() << errStr;
        qCritical() << query.lastQuery();
        return false;
    }

    return true;
}

bool updateDB(const Dive::MinimalDiver &diver, int diveId, QSqlDatabase db, QString table, bool checkExistence)
{
    if(checkExistence)
    {
        if(!exists(diver,diveId,db,table))//if the dive doesn't exist
            return false;
    }

    static const QString queryStr{"UPDATE %1 SET "
                                  "diveType = ? "
                                  "WHERE %1.diveId = ? AND %1.diverId = ?"};

    QSqlQuery query{db};
    query.prepare(queryStr.arg(table));
    query.addBindValue(to_string(diver.type));
    query.addBindValue(diveId);
    query.addBindValue(diver.id);
    query.exec();

    auto err{query.lastError()};
    if(err.type() != QSqlError::ErrorType::NoError)
    {
        QString errStr{QString{"%0 : SQL error : %1"}.arg(__func__,err.text())};
        qCritical() << errStr;
        return false;
    }

    return true;
}


Dive::MinimalDiver readMinimalDiverFromDB(int diverId, int diveId, QSqlDatabase db, QString table)
{
    static const QString queryStr{"SELECT * FROM %1 WHERE diveId=? AND diverId=?"};
    QSqlQuery query{db};
    query.prepare(queryStr.arg(table));
    query.addBindValue(diveId);
    query.addBindValue(diverId);
    query.exec();

    Dive::MinimalDiver out{};

    if(!query.next())//if nothing was found
    {
        if(enableDebug)
        {
            qDebug() << __func__ << " : No MiniMalDive found with diveId : " << diveId << "  and  diverId : " << diverId;
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

    int currentIndex{1};//skip diveId value which will not be used

//    diveId = query.value(currentIndex++).value<int>();
    out.id = query.value(currentIndex++).value<int>();
    out.type = diveTypefrom_string(query.value(currentIndex++).toString());

    return out;
}

bool exists(const Dive::MinimalDiver& diver, int diveId, QSqlDatabase db, const QString &table)
{
    return db::queryExist(db,"SELECT * FROM %0 WHERE diveId=? AND diverId=?",{table},{diveId,diver.id});
}

bool storeInDB(const Dive::MinimalDiver& diver,int diveId, QSqlDatabase db, const QString &table)
{
    auto exist{exists(diver,diveId,db,table)};
    if(!exist)//if the object doesn't exist
    {
        auto success = addToDB(diver,diveId,db,table);
        return success;
    }
    //if the diver exists
    return updateDB(diver,diveId,db,table);
}

bool removeFromDBNotIn(const QVector<Dive::MinimalDiver>& listOfDiversToKeep,int diveId,QSqlDatabase db,const QString& table)
{
    QString idList{};
    for(const auto& e : listOfDiversToKeep)
    {
        idList += QString::number(e.id)+',';
    }
    idList.chop(1);//remove last ','

    static const QString queryStr{"DELETE FROM %0 WHERE diveId=? AND diverId NOT IN (%1)"};
    QSqlQuery query{db};
    query.prepare(queryStr.arg(table,std::move(idList)));
    query.addBindValue(diveId);
    query.exec();

    auto err{query.lastError()};
    if(err.type() != QSqlError::ErrorType::NoError)
    {
        QString errStr{QString{"%0:%1:%2 : SQL error : %3"}.arg(__FILE__,_LINE_,__func__,err.text())};
        qCritical() << errStr;
        return false;
    }
    return true;
}

} // namespace info
