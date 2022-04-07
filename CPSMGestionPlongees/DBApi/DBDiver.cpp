#include "DBDiver.hpp"

#include "../global.hpp"

#include "DBApi/DataStructs.hpp"
#include "DBApi/Database.hpp"
#include "DBApi/Generic.hpp"
#include "DBApi/DBAddress.hpp"
#include "DBApi/DBApi.hpp"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include <QString>

#include <QDebug>

namespace db
{

data::Diver extractDiverFromQuery(const QSqlQuery& query,int offset){
    data::Diver out{};
    int currentIndex{offset};
    out.id = query.value(currentIndex++).value<int>();
    out.firstName = query.value(currentIndex++).value<QString>();
    out.lastName = query.value(currentIndex++).value<QString>();
    out.birthDate = QDate::fromString(query.value(currentIndex++).value<QString>(),global::format_date);
    out.email = query.value(currentIndex++).value<QString>();
    out.phoneNumber = query.value(currentIndex++).value<QString>();
    out.address = db::readAddressFromDB(query.value(currentIndex++).value<int>(),QSqlDatabase::database(),global::table_diversAddresses);
    out.licenseNumber = query.value(currentIndex++).value<QString>();
    out.certifDate = QDate::fromString(query.value(currentIndex++).value<QString>(),global::format_date);
    out.diverLevelId = query.value(currentIndex++).value<int>();
    out.member = query.value(currentIndex++).value<bool>();
    out.diveCount = query.value(currentIndex++).value<int>();
    out.paidDives = query.value(currentIndex++).value<int>();
    out.gear_regulator = query.value(currentIndex++).value<bool>();
    out.gear_suit = query.value(currentIndex++).value<bool>();
    out.gear_computer = query.value(currentIndex++).value<bool>();
    out.gear_jacket = query.value(currentIndex++).value<bool>();
    return out;
}

data::Diver extractFullDiverFromQuery(const QSqlQuery& query)
{
    qDebug() << __CURRENT_PLACE__;
    qDebug() << "Query size : " <<query.size();
    auto out{extractDiverFromQuery(query,1)};
    out.diveCount = query.value(0).value<int>();
    qDebug() << __CURRENT_PLACE__ << out.diveCount;
    return out;
}

data::Diver readDiverFromDB(int id, QSqlDatabase db, const QString& diversTable,const QString& diveMembersTable)
{
    return readFromDB<data::Diver>(db,extractFullDiverFromQuery,
//                                   "SELECT COUNT(%0.diveId),* FROM %1 WHERE id=?"
                                   "SELECT COUNT(%0.diveId),* FROM %1 LEFT JOIN "
                                   "%0 ON %0.diverId = %1.id WHERE "
                                   "%1.id=? GROUP BY %1.id;",{diveMembersTable,diversTable},{id});
}

/*!
 * \brief readDiverLFromDB : read a diver list from DB
 * \param idList : list of divers id to retrieve
 * \param db : source database
 * \param table : divers table
 * \return a list of divers
 */
QVector<data::Diver> readDiverLFromDB(QVector<int> idList, QSqlDatabase db, const QString& diversTable,const QString& diveMembersTable)
{
    auto [str,values]{db::prepRequestListFilter(idList)};
    return readLFromDB<data::Diver>(db,extractFullDiverFromQuery,
                                    "SELECT COUNT(%0.diveId),* FROM %1 LEFT JOIN "
                                    "%0 ON %0.diverId = %1.id WHERE "
                                    "%1.id IN %2 GROUP BY %1.id;",{diveMembersTable,diversTable,str},values);
}

int exists(const data::Diver& a,QSqlDatabase db,const QString& table)
{
    return (db::queryExist(db,"SELECT * FROM %0 WHERE id=?",{table},{a.id}))?a.id:-1;
}

int storeInDB(data::Diver &a, QSqlDatabase db, const QString &diverTable)
{
    auto existBefore{exists(a,db,diverTable)};

    //cf https://stackoverflow.com/questions/3634984/insert-if-not-exists-else-update;

    static QString queryStr{"INSERT INTO %1(id,"
                            "firstName,"
                            "lastName,"
                            "birthDate,"
                            "email,"
                            "phoneNumber,"
                            "memberAddressId,"
                            "licenseNumber,"
                            "certifDate,"
                            "diverLevelId,"
                            "member,"
                            "diveCount,"
                            "paidDives,"
                            "gear_regulator,"
                            "gear_suit,"
                            "gear_computer,"
                            "gear_jacket"
                            ") VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?) "
                            "ON CONFLICT(id) DO UPDATE SET "
                            "firstName=excluded.firstName,"
                            "lastName=excluded.lastName,"
                            "birthDate=excluded.birthDate,"
                            "email=excluded.email,"
                            "phoneNumber=excluded.phoneNumber,"
                            "memberAddressId=excluded.memberAddressId,"
                            "licenseNumber=excluded.licenseNumber,"
                            "certifDate=excluded.certifDate,"
                            "diverLevelId=excluded.diverLevelId,"
                            "member=excluded.member,"
                            "diveCount=excluded.diveCount,"
                            "paidDives=excluded.paidDives,"
                            "gear_regulator=excluded.gear_regulator,"
                            "gear_suit=excluded.gear_suit,"
                            "gear_computer=excluded.gear_computer,"
                            "gear_jacket=excluded.gear_jacket"
                            ";"};

    int addressId{storeInDB(a.address,db,global::table_diversAddresses)};

    if(addressId < 0)
    {
        QString errStr{QString{"%0 : SQL error : %1"}.arg(__CURRENT_PLACE__," : Cannot store address for diver")};
        qCritical() << errStr;
        return -1;
    }

    a.address.id = addressId;

    QSqlQuery query{db};
    query.prepare(queryStr.arg(diverTable));
    query.addBindValue((a.id < 0)?QVariant(QVariant::Int):a.id);
    query.addBindValue(a.firstName);
    query.addBindValue(a.lastName);
    query.addBindValue(a.birthDate.toString(global::format_date));
    query.addBindValue(a.email);
    query.addBindValue(a.phoneNumber);
    query.addBindValue(a.address.id);
    query.addBindValue(a.licenseNumber);
    query.addBindValue(a.certifDate);
    query.addBindValue(a.diverLevelId);
    query.addBindValue(a.member);
    query.addBindValue(a.diveCount);
    query.addBindValue(a.paidDives);
    query.addBindValue(a.gear_regulator);
    query.addBindValue(a.gear_suit);
    query.addBindValue(a.gear_computer);
    query.addBindValue(a.gear_suit);
    query.exec();

    auto err{query.lastError()};
    if(err.type() != QSqlError::ErrorType::NoError)
    {
        QString errStr{QString{"%0 : SQL error : %1"}.arg(__CURRENT_PLACE__,err.text())};
        qCritical() << "-------";
        qCritical() << errStr;
        qCritical() << query.boundValues();
        qCritical() << query.executedQuery();
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
        auto id{getLastInsertId(db,diverTable)};
        return id;
    }
    return a.id;
}

void removeAllFromDiver(int id, QSqlDatabase db, const QString& table)
{
    auto tempQueryResult{db::querySelect(db,"SELECT memberAddressId FROM %0 WHERE id=?",{table},{id})};
    if(tempQueryResult.size() != 1)
        return;

    int addressId{tempQueryResult[0][0].toInt()};

    //the number of divers that share this address
    auto diverCountWithAddress{db::queryCount(db,"SELECT * FROM %0 WHERE memberAddressId=?",{table},{addressId})};
    if(diverCountWithAddress <= 1)//if the address is not used by any other diver
        //remove address
        db::querySelect(db,"DELETE FROM %0 WHERE id=?",{global::table_diversAddresses},{addressId});

    //remove diver
    db::querySelect(db,"DELETE FROM %0 WHERE id=?",{table},{id});
}

} // namespace db
