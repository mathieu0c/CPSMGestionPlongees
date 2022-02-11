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

int addToDB(const data::Address& a,QSqlDatabase db,QString table)
{
    if(!db.isOpen())
    {
        QString errMsg{QString{"data::Members : %1 : database must be opened before being accessed"}.arg(__func__)};

        if(enableDebug)
            qCritical() << errMsg;
        throw std::runtime_error(errMsg.toStdString());
        return false;
    }

    QString strQuery{"INSERT INTO %1(address,"
                     "postalCode,"
                     "city"
                     ") VALUES (?,?,?)"};
    QSqlQuery query{db};
    query.prepare(strQuery.arg(table));
    query.addBindValue(a.address);
    query.addBindValue(a.postalCode);
    query.addBindValue(a.city);
    query.exec();

    auto err = query.lastError();
    if(err.type() != QSqlError::ErrorType::NoError)
    {
        QString errStr{QString{"%0 : SQL error : %1"}.arg(__func__,err.text())};
        qCritical() << errStr;
        return -1;
    }

    return getLastInsertId(db,table);
}

bool updateDB(const data::Address& a,QSqlDatabase db,QString table,bool checkExistence)
{
    if(checkExistence)
    {
        if(exists(a,db,table) == -1)//if the address doesn't exist
            return false;
    }

    static const QString queryStr{"UPDATE %1 SET "
                                  "address = ?,"
                                  "postalCode = ?,"
                                  "city = ? "
                                  "WHERE %1.id = ?"};

    QSqlQuery query{db};
    query.prepare(queryStr.arg(table));
    query.addBindValue(a.address);
    query.addBindValue(a.postalCode);
    query.addBindValue(a.city);
    query.addBindValue(a.id);
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
        return getLastInsertId(db,addressTable);
    }
    return a.id;
}

} // namespace db
