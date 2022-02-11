#include "DBDiver.hpp"

#include "../global.hpp"

#include "DBApi/DataStructs.hpp"
#include "DBApi/Database.hpp"
#include "DBApi/Generic.hpp"
#include "DBApi/DBAddress.hpp"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include <QString>

#include <QDebug>

namespace db
{

int addToDB(data::Diver& diver, QSqlDatabase db, QString table)
{
    using db::storeInDB;
    if(!db.isOpen())
    {
        QString errMsg{QString{"data::Members : %1 : database must be opened before being accessed"}.arg(__func__)};

        if(db::enableDebug)
            qCritical() << errMsg;
        throw std::runtime_error(errMsg.toStdString());
        return false;
    }

    int addressId{storeInDB(diver.address,db,global::table_diversAddresses)};
    diver.address.id = addressId;

    QString strQuery{"INSERT INTO %1(firstName,"
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
                     ") VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"};
    QSqlQuery query{db};
    query.prepare(strQuery.arg(table));
    query.addBindValue(diver.firstName);
    query.addBindValue(diver.lastName);
    query.addBindValue(diver.birthDate.toString(global::format_date));
    query.addBindValue(diver.email);
    query.addBindValue(diver.phoneNumber);
    query.addBindValue(diver.address.id);
    query.addBindValue(diver.licenseNumber);
    query.addBindValue(diver.certifDate.toString(global::format_date));
    query.addBindValue(diver.diverLevelId);
    query.addBindValue(diver.member);
    query.addBindValue(diver.diveCount);
    query.addBindValue(diver.paidDives);
    query.addBindValue(diver.gear_regulator);
    query.addBindValue(diver.gear_suit);
    query.addBindValue(diver.gear_computer);
    query.addBindValue(diver.gear_jacket);
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

bool updateDB(data::Diver& diver,QSqlDatabase db,QString table,bool checkExistence)
{
    using db::storeInDB;
    if(checkExistence)
    {
        if(exists(diver,db,table) == -1)//if the diver doesn't exist
            return false;
    }

    static const QString queryStr{"UPDATE %1 SET "
                                  "firstName = ?,"
                                  "lastName = ?,"
                                  "email = ?,"
                                  "phoneNumber = ?,"
                                  "memberAddressId = ?,"
                                  "licenseNumber = ?,"
                                  "certifDate = ?,"
                                  "diverLevelId = ?,"
                                  "member = ?,"
                                  "diveCount = ?,"
                                  "paidDives = ?,"
                                  "gear_regulator = ?,"
                                  "gear_suit = ?,"
                                  "gear_computer = ?,"
                                  "gear_jacket = ? "
                                  "WHERE %1.id = ?"};

    int addressId{storeInDB(diver.address,db,global::table_diversAddresses)};
    diver.address.id = addressId;

    QSqlQuery query{db};
    query.prepare(queryStr.arg(table));
    query.addBindValue(diver.firstName);
    query.addBindValue(diver.lastName);
    query.addBindValue(diver.email);
    query.addBindValue(diver.phoneNumber);
    query.addBindValue(addressId);
    query.addBindValue(diver.licenseNumber);
    query.addBindValue(diver.certifDate.toString(global::format_date));
    query.addBindValue(diver.diverLevelId);
    query.addBindValue(diver.member);
    query.addBindValue(diver.diveCount);
    query.addBindValue(diver.paidDives);
    query.addBindValue(diver.gear_regulator);
    query.addBindValue(diver.gear_suit);
    query.addBindValue(diver.gear_computer);
    query.addBindValue(diver.gear_jacket);
    query.addBindValue(diver.id);
    query.exec();

    auto err{query.lastError()};
    if(err.type() != QSqlError::ErrorType::NoError)
    {
        QString errStr{QString{"%0 : SQL error : %1"}.arg(__CURRENT_PLACE__,err.text())};
        qCritical() << errStr;
        return false;
    }

    return true;
}

data::Diver readDiverFromDB(int id, QSqlDatabase db, QString table)
{
    static const QString queryStr{"SELECT * FROM %1 WHERE id=?"};
    QSqlQuery query{db};
    query.prepare(queryStr.arg(table));
    query.addBindValue(id);
    query.exec();

    data::Diver out{};


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
        QString errStr{QString{"%0 : SQL error : %1"}.arg(__CURRENT_PLACE__,err.text())};
        qCritical() << errStr;
        return out;
    }

    int currentIndex{};

    out.id = query.value(currentIndex++).value<int>();
    out.firstName = query.value(currentIndex++).value<QString>();
    out.lastName = query.value(currentIndex++).value<QString>();
    out.birthDate = QDate::fromString(query.value(currentIndex++).value<QString>(),global::format_date);
    out.email = query.value(currentIndex++).value<QString>();
    out.phoneNumber = query.value(currentIndex++).value<QString>();
    out.address = db::readAddressFromDB(query.value(currentIndex++).value<int>(),db,global::table_diversAddresses);
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

int exists(const data::Diver& a,QSqlDatabase db,const QString& table)
{
    return (db::queryExist(db,"SELECT * FROM %0 WHERE diveId=? AND diverId=?",{table},{a.id}))?a.id:-1;
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
