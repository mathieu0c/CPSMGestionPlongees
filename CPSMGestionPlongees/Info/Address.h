#ifndef INFO_ADDRESS_H
#define INFO_ADDRESS_H

#include <QString>
#include <QSqlDatabase>

#include <QDebug>

namespace info
{

struct Address
{
    int id{-1};
    QString address{};
    QString postalCode{};
    QString city{};
};

QDebug operator<<(QDebug debug, const Address& a);

//return the id of the element added. -1 if failed
int addToDB(const Address& a,QSqlDatabase db,QString table);

bool updateDB(const Address& a,QSqlDatabase db,QString table,bool checkExistence = false);

Address readAddressFromDB(int id, QSqlDatabase db, QString table);

//search if the address already exists and return id if true, -1 otherwise
int exists(const Address& a, QSqlDatabase db, const QString &table);

//alter data if existing
//add data if not
//return data id
//this function may change a.id
int storeInDB(Address &a, QSqlDatabase db, const QString &table);

QString to_string(const Address& address);

}//namespace info

#endif // INFO_ADDRESS_H
