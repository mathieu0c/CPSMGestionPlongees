#ifndef DB_DBAPI_HPP
#define DB_DBAPI_HPP

#include <QSqlDatabase>
#include <QString>
#include <QVector>

#include "DBApi/Database.hpp"

#include <DBApi/DataStructs.hpp>

namespace db {

data::DivingSite readDivingSiteFromDB(int id, QSqlDatabase db, const QString& table);

//search if the address already exists and return id if true, -1 otherwise
int exists(const data::DivingSite& a, QSqlDatabase db, const QString &table);

//alter data if existing
//add data if not
//return provided id
//Note : this function does not change the id stored in the object
int storeInDB(const data::DivingSite &a, QSqlDatabase db, const QString &addressTable);


//---------- Dives

int updateDiversDiveCount(const QVector<data::DiveMember>& divers,QSqlDatabase db,const QString& diveMembersTable,
                          const QString& diversTable);

/*!
 * \brief exists : Checks if a DiveMember exists in the DB based on diverId and diveId
 * \param diver to check
 * \param db : database to search for in
 * \param table : DivesMembers table
 * \return diver ID if exist, -1 otherwise
 */
inline
int exists(const data::DiveMember& diver,QSqlDatabase db, const QString& table)
{
    return (db::queryExist(db,"SELECT * FROM %1 WHERE %1.diverId = ? AND %1.diveId = ?",{table},{diver.diveId,diver.diverId}))?diver.diverId:-1;
}

int storeInDB(const data::DiveMember& divers, QSqlDatabase db, const QString& diveMembersTable);
/*!
 * \brief storeInDB
 * \param divers who participated in a dive list to store
 * \param db
 * \param diveMembersTable
 * \return the count of DiveMembers if succedeed, -1 otherwise
 */
int storeInDB(const QVector<data::DiveMember>& divers, QSqlDatabase db, const QString& diveMembersTable);

} // namespace db

#endif // DB_DBAPI_HPP
