#ifndef DB_GENERIC_HPP
#define DB_GENERIC_HPP

#include <QSqlDatabase>
#include <QVariant>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>

#include <type_traits>//debugging purpose
#include <concepts>
#include <QDebug>

#include "../global.hpp"

#include "DBApi/DataStructs.hpp"
#include "DBApi/DBAddress.hpp"
#include "DBApi/DBDive.hpp"
#include "DBApi/DBDiver.hpp"

namespace db
{

template<typename T> requires (!std::is_same<T,data::Address>() && !std::is_same<T,data::Diver>()
                               && !std::is_same<T,data::Dive>())
int storeInDB(T &object, QSqlDatabase db, const QString &table)
{

//    auto lambdaDebug{
//        [&](const QString& str)
//        {
//            if constexpr (std::is_same_v<T, Dive>)
//            {
//                qDebug() << __FILE__ << ":" << __func__ << ":" << __LINE__ << ": " << str;
//            }
//        }
//    };

    auto id{exists(object,db,table)};
    if(id == -1)//if the object doesn't exist
    {

        id = addToDB(object,db,table);
        object.id = id;
        return id;
    }
    else
    {
        if(object.id == -1)
            object.id = id;

        if(!updateDB(object,db,table))
            return -1;
        return id;
    }

    return -1;
}

inline
bool updateDBField(int id, const QString& field, QVariant value, QSqlDatabase db, const QString& table)
{
    static const QString queryStr{"UPDATE %1 SET "
                                  "%2 = ? "
                                  "WHERE %1.id = ?"};
    QSqlQuery query{db};
    query.prepare(queryStr.arg(table,field));
    query.addBindValue(value);
    query.addBindValue(id);
    query.exec();

    auto err{query.lastError()};
    if(err.type() != QSqlError::ErrorType::NoError)
    {
        QString errStr{QString{"%0 : SQL error : %1"}.arg(__CURRENT_PLACE__,err.text())};
        qCritical() << errStr;
        qDebug() << "Query : " << query.executedQuery();
        for(const auto& e : query.boundValues())
        {
            qDebug() << "    Bound : " << e;
        }
        return false;
    }

    return true;
}

}//namespace db

#endif // DB_GENERIC_HPP
