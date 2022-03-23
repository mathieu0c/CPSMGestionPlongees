#include "DBDive.hpp"
#include "../global.hpp"

#include "DBApi/DBApi.hpp"
#include "DBApi/Database.hpp"
#include "DBApi/DBDiver.hpp"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include <algorithm>
#include <unordered_map>

#include <QDebug>
#include "Debug/global.hpp"
#include "Debug/ScopeTimer.hpp"

namespace db {

QVector<data::DiveMember> readDiveMembersFromDB(int diveId,QSqlDatabase db, const QString& diveMembersTable, const QString& diversTable)
{
    const auto diversTableColumnCount{
        QSqlQuery(QString{"SELECT * FROM %1 LIMIT 1"}.arg(diversTable)).record().count()
    };
    return readLFromDB<data::DiveMember>(db,[&](const QSqlQuery& query){
            data::DiveMember member{};
            int diveMemberQueryOffset{diversTableColumnCount+1};//The number of columns in the table "Divers"
                //because DiveMember data will be accessible after "Diver"'s data
            member.fullDiver = extractDiverFromQuery(query);
            member.diveId = diveId;
            member.diverId = member.fullDiver.id;
            diveMemberQueryOffset++;//skip diver id in diveMember table
            member.type = data::diveTypefrom_string(query.value(diveMemberQueryOffset++).value<QString>());
        return member;
    },"SELECT * FROM %0 INNER JOIN %1 ON %0.id = %1.diverId WHERE %1.diveId = ?",
    {diversTable,diveMembersTable},{diveId});
}

QVector<data::DiveMember> readDiveMembersFromDB(int diveId,QVector<int> diversId,QSqlDatabase db,const QString& diversTable)
{
    const auto diversTableColumnCount{
        QSqlQuery(QString{"SELECT * FROM %1 LIMIT 1"}.arg(diversTable)).record().count()
    };
    auto [strIds,idsValues]{db::prepRequestListFilter(diversId)};
    return readLFromDB<data::DiveMember>(db,[&](const QSqlQuery& query){
            data::DiveMember member{};
            int diveMemberQueryOffset{diversTableColumnCount+1};//The number of columns in the table "Divers"
                //because DiveMember data will be accessible after "Diver"'s data
            member.fullDiver = extractDiverFromQuery(query);
            member.diveId = diveId;
            member.diverId = member.fullDiver.id;
            member.type = data::diveTypefrom_string(query.value(diveMemberQueryOffset++).value<QString>());
        return member;
    },"SELECT * FROM %0 WHERE %0.id IN %1",
    {diversTable,strIds},idsValues);
}


bool removeAllFromDB(const data::Dive& dive,QSqlDatabase db, const QString& table)
{
    QSqlQuery{"BEGIN TRANSACTION;",db};

    //------------------------- Delete from DivesMembers table
    QString idList{};
//    for(const auto& e : dive.divers)
//    {
//        idList += QString::number(e.id)+',';
//    }
//    idList.chop(1);//remove last ','

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
int addToDB(data::Dive &dive, QSqlDatabase db, QString table)
{
    if(!db.isOpen())
    {
        QString errMsg{QString{"data::Dive : %1 : database must be opened before being accessed"}.arg(__func__)};

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
        QString errStr{QString{"data::Dive : %0 : SQL error : %1 :"}.arg(__func__,err.text())};
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
//    for(const auto& diver : dive.divers)
//    {
//        if(enableDebug)
//            qDebug() << "INSERT INTO : " << global::table_divesMembers;
//        auto minDiverSuccess{addToDB(diver,dive.id,db,global::table_divesMembers)};

//        if(!minDiverSuccess)
//        {
//            QString errStr{QString{"%0 : SQL error : %1"}.arg(__func__,"Minimal Dive addition")};
//            qCritical() << errStr;
//            QSqlQuery{"ROLLBACK;",db};
//            return -1;
//        }
//    }

    if(lastId != -1)
        QSqlQuery{"COMMIT;",db};
    else
        QSqlQuery{"ROLLBACK;",db};


    return lastId;
}

bool updateDB(data::Dive& dive,QSqlDatabase db,QString table,bool checkExistence)
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
        QString errStr{QString{"data::Dive : %0 : SQL error : %1 :"}.arg(__func__,err.text())};
        qCritical() << errStr;
        qCritical() << query.lastQuery();
        QSqlQuery{"ROLLBACK;",db};
        return -1;
    }

//    for(const auto& diver : dive.divers)
//    {
//        auto minDiverSuccess{storeInDB(diver,dive.id,db,global::table_divesMembers)};

//        if(!minDiverSuccess)
//        {
//            QString errStr{QString{"%0 : SQL error : %1"}.arg(__func__,"Minimal Dive addition")};
//            qCritical() << errStr;
//            QSqlQuery{"ROLLBACK;",db};
//            return false;
//        }
//    }

//    if(!removeFromDBNotIn(dive.divers,dive.id,db,global::table_divesMembers))
//    {
//        QString errStr{QString{"%0:%1:%2 : SQL error : %3"}.arg(__FILE__,_LINE_,__func__,"Minimal Dive addition")};
//        qCritical() << errStr;
//        QSqlQuery{"ROLLBACK;",db};
//        return false;
//    }

    QSqlQuery{"COMMIT;",db};

    return true;
}

data::Dive readDiveFromDB(int id, QSqlDatabase db, const QString& diveTable,
                          const QString& divingSiteTable, const QString& diveMembersTable,
                          const QString& diversTable)
{
    auto out{readFromDB<data::Dive>(db,[&](const QSqlQuery& query){
        data::Dive out{};
        out.id = query.value(0).value<int>();
        out.date = QDate::fromString(query.value(1).value<QString>(),global::format_date);
        out.time = QTime::fromString(query.value(2).value<QString>(),global::format_time);
        out.diveSiteId = query.value(3).value<int>();
        out.site.id = query.value(3).value<int>();
        out.site.name = query.value(4).value<QString>();
        return out;
    },"SELECT %0.id,date,time,diveSiteId,name FROM %0 INNER JOIN %1 ON %0.diveSiteId=%1.id WHERE %0.id = ?",
      {diveTable,divingSiteTable},{id})};

    //SELECT * FROM Divers INNER JOIN DivesMembers ON Divers.id = DivesMembers.diverId WHERE DivesMembers.diveId = 1

    out.diver = readDiveMembersFromDB(out.id,db,diveMembersTable,diversTable);

//    qDebug() << out;

    return out;
}

int storeInDB(data::Dive& dive, QSqlDatabase db, const QString &diveTable, const QString& diversTable,
              const QString& diveMembersTable)
{
    //Perf (release) :
    //Approx 4.4 - 6.2 ms when only the dive is udpated (not the divers neither diveMembers)
    //When in max eco mode
//    debug::ScopeTimer __tim{__CURRENT_PLACE__};
    auto existBefore{exists(dive,db,diveTable)};

    QSqlQuery{"BEGIN TRANSACTION;",db};//just in case something fail after

    //cf https://stackoverflow.com/questions/3634984/insert-if-not-exists-else-update;
    static QString queryStr{"INSERT INTO %1(id,"
                            "date,"
                            "time,"
                            "diveSiteId"
                            ") VALUES (?,?,?,?) "
                            "ON CONFLICT(id) DO UPDATE SET "
                            "date=excluded.date,"
                            "time=excluded.time,"
                            "diveSiteId=excluded.diveSiteId;"};

    QSqlQuery query{db};
    query.prepare(queryStr.arg(diveTable));
    query.addBindValue((dive.id < 0)?QVariant(QVariant::Int):dive.id);
    query.addBindValue(dive.date.toString(global::format_date));
    query.addBindValue(dive.time.toString(global::format_time));
    query.addBindValue(dive.diveSiteId);
    query.exec();

    auto err{query.lastError()};
    if(err.type() != QSqlError::ErrorType::NoError)
    {
        QString errStr{QString{"%0 : SQL error : %1"}.arg(__CURRENT_PLACE__,err.text())};
        qCritical() << errStr;
        debug::debugQuery(query,__CURRENT_PLACE__);
        QSqlQuery{"ROLLBACK;",db};//cancel transaction
        return -1;
    }


    //remove deleted divers from the dive
    auto existingDivers{readLFromDB<int>(db,[&](const QSqlQuery& query){
        int out{query.value(0).toInt()};
        return out;
    },"SELECT diverId FROM %0 WHERE %0.diveId = ?",
      {diveMembersTable},{dive.id})};
    QVector<QVariant> diversToRemoveFromDBDive{};
    diversToRemoveFromDBDive.reserve(existingDivers.size());
    QString removeListArgs{"("};//"?,?,?,?" with the right count of values corresponding to the number of divers to remove
    for(const auto& diverId : existingDivers)
    {
        if(std::find_if(dive.diver.cbegin(),dive.diver.cend(),[&](auto e){return e.diverId == diverId;})
                        == dive.diver.cend())//if existing diver in the db isn't found in the dive's divers list
        {
            diversToRemoveFromDBDive.push_back(diverId);//say that we need to remove it
            removeListArgs.append("?,");
        }
    }
    if(removeListArgs.endsWith(','))
        removeListArgs.resize(removeListArgs.size()-1);
    removeListArgs += ")";

    auto successRemoveDivers{db::queryDelete(db,"DELETE FROM %0 WHERE diveId=? AND diverId IN %1",
                                             {diveMembersTable,removeListArgs},
                                             QVector<QVariant>{dive.id}+diversToRemoveFromDBDive)};
    if(!successRemoveDivers)
    {
        QString errStr{QString{"%0 : SQL error : Cannot update DivesMembers table (delete diveMembers)"}.arg(__CURRENT_PLACE__)};
        qCritical() << errStr;
        QSqlQuery{"ROLLBACK;",db};//cancel transaction
        return -1;
    }

    //add new diveMembers to db
    auto addDiveMembersResult{storeInDB(dive.diver,db,diveMembersTable)};
    if(addDiveMembersResult < 0)
    {
        QString errStr{QString{"%0 : SQL error : Cannot update DivesMembers table"}.arg(__CURRENT_PLACE__)};
        qCritical() << errStr;
        QSqlQuery{"ROLLBACK;",db};//cancel transaction
        return -1;
    }



//    debug::debugQuery(query,__CURRENT_PLACE__);

   if(updateDiversDiveCount(dive.diver,db,diveMembersTable,diversTable) < 0)//update diveCount
   {
       QString errStr{QString{"%0 : SQL error : Cannot update divers DiveCount"}.arg(__CURRENT_PLACE__)};
       qCritical() << errStr;
       QSqlQuery{"ROLLBACK;",db};//cancel transaction
       return -1;
   }

    auto returnId{-1};

    if(existBefore < 0)
    {
        auto id{getLastInsertId(db,diveTable)};
        returnId = id;
    }
    else
    {
        returnId = dive.id;
    }


    QSqlQuery{"COMMIT;",db};

    return returnId;
}

int exists(const data::Dive& a,QSqlDatabase db,const QString& table)
{
    return (db::queryExist(db,"SELECT id FROM %1 WHERE %1.id",{table},{a.id}))?a.id:-1;
}


//-------------------------------------------------------------------------------------
//------------------------  MinimalDiver funcs  ---------------------------------------
//-------------------------------------------------------------------------------------

QDebug operator<<(QDebug debug, const data::Dive::MinimalDiver& d)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "MinimalDiver{\nid : " << d.id << "\n";
    debug.nospace() << "Type : " << to_string(d.type) << "\n";
    debug.nospace() << "};";

    return debug;
}

bool addToDB(const data::Dive::MinimalDiver& diver, int diveId, QSqlDatabase db, QString table)
{
//    if(!db.isOpen())
//    {
//        QString errMsg{QString{"data::Dive::addToDB(MinimalDiver) : %1 : database must be opened before being accessed"}.arg(__func__)};

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

bool updateDB(const data::Dive::MinimalDiver &diver, int diveId, QSqlDatabase db, QString table, bool checkExistence)
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


data::Dive::MinimalDiver readMinimalDiverFromDB(int diverId, int diveId, QSqlDatabase db, QString table)
{
    static const QString queryStr{"SELECT * FROM %1 WHERE diveId=? AND diverId=?"};
    QSqlQuery query{db};
    query.prepare(queryStr.arg(table));
    query.addBindValue(diveId);
    query.addBindValue(diverId);
    query.exec();

    data::Dive::MinimalDiver out{};

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
    out.type = data::diveTypefrom_string(query.value(currentIndex++).toString());

    return out;
}

bool exists(const data::Dive::MinimalDiver& diver, int diveId, QSqlDatabase db, const QString &table)
{
    return db::queryExist(db,"SELECT * FROM %0 WHERE diveId=? AND diverId=?",{table},{diveId,diver.id});
}

bool storeInDB(const data::Dive::MinimalDiver& diver,int diveId, QSqlDatabase db, const QString &table)
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

bool removeFromDBNotIn(const QVector<data::Dive::MinimalDiver>& listOfDiversToKeep,int diveId,QSqlDatabase db,const QString& table)
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

} // namespace db
