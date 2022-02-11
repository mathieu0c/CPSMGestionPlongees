#ifndef DB_ADDRESS_HPP
#define DB_ADDRESS_HPP

//#include "DataStruct/Address.h"

#include "DBApi/DataStructs.hpp"

#include <QSqlDatabase>

namespace db
{

//return the id of the element added. -1 if failed
int addToDB(const data::Address& a,QSqlDatabase db,QString table);

bool updateDB(const data::Address& a,QSqlDatabase db,QString table,bool checkExistence = false);

data::Address readAddressFromDB(int id, QSqlDatabase db, QString table);

//search if the address already exists and return id if true, -1 otherwise
int exists(const data::Address& a, QSqlDatabase db, const QString &table);

//alter data if existing
//add data if not
//return data id
//this function may change a.id if it inserts the value in the table
int storeInDB(data::Address &a, QSqlDatabase db, const QString &addressTable);

} // namespace db

#endif // DB_ADDRESS_HPP
