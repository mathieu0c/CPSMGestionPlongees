#ifndef DB_ADDRESS_HPP
#define DB_ADDRESS_HPP

//#include "DataStruct/Address.h"

#include "DBApi/DataStructs.hpp"

#include <QSqlDatabase>

namespace db
{

data::Address readAddressFromDB(int id, QSqlDatabase db, QString table);

//search if the address already exists and return id if true, -1 otherwise
int exists(const data::Address& a, QSqlDatabase db, const QString &table);

//alter data if existing
//add data if not
//return provided id
//Note : this function does not change the id stored in the object
int storeInDB(const data::Address &a, QSqlDatabase db, const QString &addressTable);

} // namespace db

#endif // DB_ADDRESS_HPP
