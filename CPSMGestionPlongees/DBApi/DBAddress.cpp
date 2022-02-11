#include "DBAddress.hpp"

//#include "DataStruct/global.hpp"

#include "../global.hpp"

#include "DBApi/Database.hpp"

#include "DBApi/DataStructs.hpp"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include <QDebug>

namespace db
{

data::Address readAddressFromDB(int id, QSqlDatabase db, QString table)
{
    static const QString queryStr{"SELECT * FROM %1 WHERE id=?"};
    QSqlQuery query{db};
    query.prepare(queryStr.arg(table));
    query.addBindValue(id);
    query.exec();

    return readFromDB<data::Address>(db,[&](const QSqlQuery& query){
        data::Address out{};
        out.id = query.value(0).value<int>();
        out.address = query.value(1).value<QString>();
        out.postalCode = query.value(2).value<QString>();
        out.city = query.value(3).value<QString>();
        return out;
    },"SELECT * FROM %1 WHERE id=?",{table},{id});

}

int exists(const data::Address& a,QSqlDatabase db,const QString& table)
{
    return (db::queryExist(db,"SELECT id FROM %1 WHERE %1.id",{table},{a.id}))?a.id:-1;
}

int storeInDB(data::Address &a, QSqlDatabase db, const QString &addressTable)
{
    auto existBefore{exists(a,db,addressTable)};

    //cf https://stackoverflow.com/questions/3634984/insert-if-not-exists-else-update;
    static QString queryStr{"INSERT INTO %1(id,"
                            "address,"
                            "postalCode,"
                            "city"
                            ") VALUES (?,?,?,?) "
                            "ON CONFLICT(id) DO UPDATE SET "
                            "address=excluded.address,"
                            "postalCode=excluded.postalCode,"
                            "city=excluded.city;"};

    QSqlQuery query{db};
    query.prepare(queryStr.arg(addressTable));
    query.addBindValue((a.id < 0)?QVariant(QVariant::Int):a.id);
    query.addBindValue(a.address);
    query.addBindValue(a.postalCode);
    query.addBindValue(a.city);
    query.exec();

//    qDebug() << "-----------------------------";
//    qDebug() << __CURRENT_PLACE__<< ": " << query.lastQuery();
//    for(const auto& e : query.boundValues())
//    {
//        qDebug() << e;
//    }

    if(existBefore < 0)
    {
        auto id{getLastInsertId(db,addressTable)};
        a.id = id;
        return id;
    }
    return a.id;
}

} // namespace db
