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

    query.prepare(QString{"SELECT last_insert_rowid() FROM %1"}.arg(table));
    auto temp{db::querySelect(db,"SELECT last_insert_rowid()",{},{})};
    if(temp.size() > 0)
        return temp[0][0].toInt();

    return -1;
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

    data::Address out{};


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

    out.id = query.value(0).value<int>();
    out.address = query.value(1).value<QString>();
    out.postalCode = query.value(2).value<QString>();
    out.city = query.value(3).value<QString>();


    return out;
}

int exists(const data::Address& a,QSqlDatabase db,const QString& table)
{
    /*auto temp{db::querySelect(db,"SELECT id FROM %1 WHERE %1.address = ? AND %1.postalCode = ? AND %1.city = ?",{table},{a.address,a.postalCode,a.city})};

    if(temp.size() > 0)
    {
        return temp[0][0].toInt();
    }
    return -1;*/
    auto temp{db::querySelect(db,"SELECT id FROM %1 WHERE %1.id = ?",{table},{a.id})};

    if(temp.size() > 0)
    {
        return temp[0][0].toInt();
    }
    return -1;
}

//int storeInDB(Address& a, QSqlDatabase db, const QString &table)
//{
//    auto id{exists(a,db,table)};
//    if(id == -1 && a.id == -1)//if the address doesn't exist by id
//    {
//        id = addToDB(a,db,table);
//        a.id = id;

//        return id;
//    }
//    else
//    {
//        if(a.id == -1)
//            a.id = id;

//        if(!updateDB(a,db,table))
//            return -1;
//        return id;
//    }
//}

} // namespace db
