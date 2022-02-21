#ifndef DB_DIVER_H
#define DB_DIVER_H

#include "DBApi/DataStructs.hpp"

#include <QSqlDatabase>

#include <QDebug>

namespace db
{

//return the id of the element added. -1 if failed
int addToDB(data::Diver &diver, QSqlDatabase db, QString table);

//This function may change diver.address.id
bool updateDB(data::Diver &diver, QSqlDatabase db, QString table, bool checkExistence = false);


data::Diver readDiverFromDB(int id, QSqlDatabase db, QString table);

//search if the diver already exists and return id if true, -1 otherwise
int exists(const data::Diver& a, QSqlDatabase db, const QString &table);

//alter data if existing
//add data if not
//return diverId
//this function may change diver.address.id
int storeInDB(data::Diver& diver, QSqlDatabase db, const QString &table);

/*!
 * \brief removeAllFromDiver : delete a diver and it's address if not used by any other one
 * \param id
 * \param db
 * \param table
 */
void removeAllFromDiver(int id, QSqlDatabase db, const QString& table);

} // namespace db

#endif // DB_DIVER_H
