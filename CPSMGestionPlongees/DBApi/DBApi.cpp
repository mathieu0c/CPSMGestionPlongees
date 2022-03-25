#include "DBApi.hpp"

#include "../global.hpp"

#include "DBApi/Database.hpp"
#include "DBApi/DBDiver.hpp"

#include "DBApi/DataStructs.hpp"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include <QDebug>
#include "Debug/global.hpp"

namespace db {

data::DivingSite readDivingSiteFromDB(int id, QSqlDatabase db, const QString &table)
{
    return readFromDB<data::DivingSite>(db,[&](const QSqlQuery& query){
        data::DivingSite out{};
        out.id = query.value(0).value<int>();
        out.name = query.value(1).value<QString>();
        return out;
    },"SELECT * FROM %1 WHERE id=?",{table},{id});
}

//search if the address already exists and return id if true, -1 otherwise
int exists(const data::DivingSite& a, QSqlDatabase db, const QString &table)
{
    return (db::queryExist(db,"SELECT id FROM %1 WHERE %1.id=?",{table},{a.id}))?a.id:-1;
}

//alter data if existing
//add data if not
//return provided id
//Note : this function does not change the id stored in the object
int storeInDB(const data::DivingSite &a, QSqlDatabase db, const QString &addressTable)
{
    auto existBefore{exists(a,db,addressTable)};

    //cf https://stackoverflow.com/questions/3634984/insert-if-not-exists-else-update;
    static QString queryStr{"INSERT INTO %1(id,"
                            "name"
                            ") VALUES (?,?) "
                            "ON CONFLICT(id) DO UPDATE SET "
                            "name=excluded.name;"};

    QSqlQuery query{db};
    query.prepare(queryStr.arg(addressTable));
    query.addBindValue((a.id < 0)?QVariant(QVariant::Int):a.id);
    query.addBindValue(a.name);
    query.exec();

    auto err{query.lastError()};
    if(err.type() != QSqlError::ErrorType::NoError)
    {
        QString errStr{QString{"%0 : SQL error : %1"}.arg(__CURRENT_PLACE__,err.text())};
        qCritical() << errStr;
        return -1;
    }

//    debug::debugQuery(query,__CURRENT_PLACE__);

    if(existBefore < 0)
    {
        auto id{getLastInsertId(db,addressTable)};
        return id;
    }
    return a.id;
}

int updateDiversDiveCount(const QVector<data::DiveMember>& divers,QSqlDatabase db,const QString& diveMembersTable,
                          const QString& diversTable)
{
    QSqlQuery{"BEGIN TRANSACTION;",db};

    //UPDATE Divers SET diveCount = (SELECT COUNT(diveId) FROM DivesMembers WHERE diverId = 2) WHERE id = 2

    static const QString queryStr{QString{"UPDATE %1 SET "
                                          "diveCount = "
                                          "(SELECT COUNT(diveId) FROM %2 WHERE diverId = ?)"
                                          "WHERE %1.id = ?"}.arg(diversTable,diveMembersTable)};

    QSqlQuery query{db};

    for(const auto& diver : divers)
    {
        query.prepare(queryStr);
        query.addBindValue(diver.diverId);
        query.addBindValue(diver.diverId);
        query.exec();
//        debug::debugQuery(query,__CURRENT_PLACE__);

        auto err = query.lastError();
        if(err.type() != QSqlError::ErrorType::NoError)
        {
            QString errStr{QString{"%0 : SQL error : %1 :"}.arg(__CURRENT_PLACE__,err.text())};
            qCritical() << errStr;
            debug::debugQuery(query,__CURRENT_PLACE__);
            QSqlQuery{"ROLLBACK;",db};
            return -1;
        }
    }

    QSqlQuery{"COMMIT;",db};

    return 0;
}

int storeInDB(const data::DiveMember& diver,QSqlDatabase db, const QString& diveMembersTable)
{
//    auto existBefore{exists(diver,db,diveMembersTable)};
//    if(existBefore)
//    {
//        qCritical() << __CURRENT_PLACE__ << " : SQL Error : " << "DiveMember already exists in DB : " << diver;
//        return -1;
//    }

    static const QString queryStr{QString{"INSERT INTO %1(diveId,"
                            "diverId,"
                            "diveType"
                            ") VALUES (?,?,?) "
                            "ON CONFLICT(diveId,diverId) DO UPDATE SET "
                            "diveType=excluded.diveType;"}.arg(diveMembersTable)};

    QSqlQuery query{db};
    query.prepare(queryStr);
    query.addBindValue(diver.diveId);
    query.addBindValue(diver.diverId);
    query.addBindValue(to_string(diver.type));
    query.exec();

    auto err{query.lastError()};
    if(err.type() != QSqlError::ErrorType::NoError)
    {
        QString errStr{QString{"%0 : SQL error : %1"}.arg(__CURRENT_PLACE__,err.text())};
        qCritical() << errStr;
        debug::debugQuery(query,__CURRENT_PLACE__);
        return -1;
    }

    return diver.diverId;
}

int storeInDB(const QVector<data::DiveMember>& divers,QSqlDatabase db, const QString& diveMembersTable)
{
//    auto existBefore{exists(diver,db,diveMembersTable)};
//    if(existBefore)
//    {
//        qCritical() << __CURRENT_PLACE__ << " : SQL Error : " << "DiveMember already exists in DB : " << diver;
//        return -1;
//    }
    QSqlQuery{"BEGIN TRANSACTION;",db};//just in case something fail after

    static const QString queryStr{QString{"INSERT INTO %1(diveId,"
                            "diverId,"
                            "diveType"
                            ") VALUES (?,?,?) "
                            "ON CONFLICT(diveId,diverId) DO UPDATE SET "
                            "diveType=excluded.diveType;"}.arg(diveMembersTable)};

    QSqlQuery query{db};
    for(const auto& diver : divers)
    {
        query.prepare(queryStr);
        query.addBindValue(diver.diveId);
        query.addBindValue(diver.diverId);
        query.addBindValue(to_string(diver.type));
        query.exec();

        auto err{query.lastError()};
        if(err.type() != QSqlError::ErrorType::NoError)
        {
            QString errStr{QString{"%0 : SQL error"}.arg(__CURRENT_PLACE__)};
            qCritical() << errStr;
            debug::debugQuery(query,__CURRENT_PLACE__);
            QSqlQuery{"ROLLBACK;",db};//cancel transaction
            return -1;
        }
    }
    QSqlQuery{"COMMIT;",db};

    return divers.size();
}

} // namespace db
