#include "DBApi.hpp"

#include "../global.hpp"

#include "DBApi/Database.hpp"

#include "DBApi/DataStructs.hpp"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include <QDebug>

namespace db {

data::DivingSite readDivingSiteFromDB(int id, QSqlDatabase db, QString table)
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
    return (db::queryExist(db,"SELECT id FROM %1 WHERE %1.id",{table},{a.id}))?a.id:-1;
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

//    qDebug() << "-----------------------------";
//    qDebug() << __CURRENT_PLACE__<< ": " << query.lastQuery();
//    for(const auto& e : query.boundValues())
//    {
//        qDebug() << e;
//    }

    if(existBefore < 0)
    {
        auto id{getLastInsertId(db,addressTable)};
        return id;
    }
    return a.id;
}

} // namespace db
