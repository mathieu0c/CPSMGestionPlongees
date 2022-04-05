#include "DBDiverLevel.hpp"

#include "../global.hpp"

#include "DBApi/Database.hpp"
#include "DBApi/DBApi.hpp"

#include "DBApi/DataStructs.hpp"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include <QDebug>
#include "Debug/global.hpp"

namespace db {

data::DiverLevel extractDiverLevelFromQuery(const QSqlQuery& query)
{
    data::DiverLevel out{};
    out.id = query.value(0).value<int>();
    out.level = query.value(1).value<QString>();
    out.sortValue = query.value(2).value<int>();
    return out;
}

data::DiverLevel readDiverLevelFromDB(int id, QSqlDatabase db, QString table)
{
    return readFromDB<data::DiverLevel>(db,db::extractDiverLevelFromQuery,"SELECT * FROM %1 WHERE id=?",{table},{id});
}

QVector<data::DiverLevel> readDiverLevelLFromDB(QVector<int> idList, QSqlDatabase db, QString table)
{
    auto [str,values]{db::prepRequestListFilter(idList)};
    return readLFromDB<data::DiverLevel>(db,extractDiverLevelFromQuery,"SELECT * FROM %1 WHERE id IN %2",{table,str},values);
}

int exists(const data::DiverLevel& l,QSqlDatabase db,const QString& table)
{
    return (db::queryExist(db,"SELECT id FROM %1 WHERE %1.id=?",{table},{l.id}))?l.id:-1;
}

int storeInDB(const data::DiverLevel &l, QSqlDatabase db, const QString &addressTable)
{
    auto existBefore{exists(l,db,addressTable)};

    //cf https://stackoverflow.com/questions/3634984/insert-if-not-exists-else-update;
    static QString queryStr{"INSERT INTO %1(id,"
                            "level,"
                            "sortValue"
                            ") VALUES (?,?,?) "
                            "ON CONFLICT(id) DO UPDATE SET "
                            "level=excluded.level,"
                            "sortValue=excluded.sortValue;"};

    QSqlQuery query{db};
    query.prepare(queryStr.arg(addressTable));
    query.addBindValue((l.id < 0)?QVariant(QVariant::Int):l.id);
    query.addBindValue(l.level);
    query.addBindValue(l.sortValue);
    query.exec();

    auto err{query.lastError()};
    if(err.type() != QSqlError::ErrorType::NoError)
    {
        QString errStr{QString{"%0 : SQL error : %1"}.arg(__CURRENT_PLACE__,err.text())};
        qCritical() << errStr;
        debug::debugQuery(query,__CURRENT_PLACE__);
        return -1;
    }

//    debug::debugQuery(query,__CURRENT_PLACE__);

    if(existBefore < 0)
    {
        auto id{getLastInsertId(db,addressTable)};
        return id;
    }
    return l.id;
}

} // namespace db
