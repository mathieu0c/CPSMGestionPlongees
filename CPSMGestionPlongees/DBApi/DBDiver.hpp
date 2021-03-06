#ifndef DB_DIVER_H
#define DB_DIVER_H

#include "DBApi/DataStructs.hpp"

#include <QSqlDatabase>

#include <QDebug>

namespace db
{

data::Diver extractFullDiverFromQuery(const QSqlQuery& query);
data::Diver extractDiverFromQuery(const QSqlQuery& query, int offset = 0);

data::Diver readDiverFromDB(int id, QSqlDatabase db, const QString &diversTable, const QString &diveMembersTable);
QVector<data::Diver> readDiverLFromDB(QVector<int> idList, QSqlDatabase db, const QString &diversTable, const QString &diveMembersTable);

//search if the diver already exists and return id if true, -1 otherwise
int exists(const data::Diver& a, QSqlDatabase db, const QString &table);

//alter data if existing
//add data if not
//return diverId
//this function may change diver.address.id
int storeInDB(data::Diver& diver, QSqlDatabase db, const QString &diverTable);

/*!
 * \brief removeAllFromDiver : delete a diver and it's address if not used by any other one
 * \param id
 * \param db
 * \param table
 */
void removeAllFromDiver(int id, QSqlDatabase db, const QString& table);

} // namespace db

#endif // DB_DIVER_H
