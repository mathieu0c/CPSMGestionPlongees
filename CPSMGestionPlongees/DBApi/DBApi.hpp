#ifndef DB_DBAPI_HPP
#define DB_DBAPI_HPP

#include <QSqlDatabase>
#include <QString>
#include <QVector>

#include <DBApi/DataStructs.hpp>

namespace db {

data::DivingSite readDivingSiteFromDB(int id, QSqlDatabase db, QString table);

//search if the address already exists and return id if true, -1 otherwise
int exists(const data::DivingSite& a, QSqlDatabase db, const QString &table);

//alter data if existing
//add data if not
//return provided id
//Note : this function does not change the id stored in the object
int storeInDB(const data::DivingSite &a, QSqlDatabase db, const QString &addressTable);

} // namespace db

#endif // DB_DBAPI_HPP
