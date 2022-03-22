#ifndef DB_DIVE_H
#define DB_DIVE_H

#include <QSqlDatabase>
#include <QString>
#include <QVector>

#include <DBApi/DataStructs.hpp>

namespace db {

QVector<data::DiveMember> readDiveMembersFromDB(int diveId, QSqlDatabase db, const QString& diveMembersTable, const QString &diversTable);

/*!
 * \brief removeAllFromDB : Remove all component in all tables linked to dive
 * \param dive
 * \param db
 * \param table
 */
bool removeAllFromDB(const data::Dive& dive,QSqlDatabase db, const QString& table);

//return the id of the element added. -1 if failed
int addToDB(data::Dive &dive, QSqlDatabase db, QString table);

//This function may change diver.address.id
bool updateDB(data::Dive &dive, QSqlDatabase db, QString table, bool checkExistence = false);

data::Dive readDiveFromDB(int id, QSqlDatabase db, const QString &diveTable,
                          const QString &divingSiteTable, const QString &diveMembersTable,
                          const QString &diversTable);

int storeInDB(data::Dive &dive, QSqlDatabase db, const QString &diveTable, const QString &diversTable,
              const QString& diveMembersTable);

//search if the diver already exists and return id if true, -1 otherwise
int exists(const data::Dive& a, QSqlDatabase db, const QString &table);

//alter data if existing
//add data if not
//return data id
//this function may change diver.id and diver.address.id
//int storeInDB(Dive& diver, QSqlDatabase db, const QString &table);


//-------------------------------------------------------------------------------------
//------------------------  MinimalDiver funcs  ---------------------------------------
//-------------------------------------------------------------------------------------

QDebug operator<<(QDebug debug, const data::Dive::MinimalDiver& d);

bool addToDB(const data::Dive::MinimalDiver& diver, int diveId, QSqlDatabase db, QString table);

bool updateDB(const data::Dive::MinimalDiver &diver, int diveId, QSqlDatabase db, QString table, bool checkExistence = false);

data::Dive::MinimalDiver readMinimalDiverFromDB(int diverId, int diveId, QSqlDatabase db, QString table);

//search if the minimalDive exist
bool exists(const data::Dive::MinimalDiver& diver, int diveId, QSqlDatabase db, const QString &table);

bool storeInDB(const data::Dive::MinimalDiver& diver,int diveId, QSqlDatabase db, const QString &table);

/*!
 * \brief removeFromDBNotIn : remove MinimalDivers participating to the dive with id=diveId from DB
 *                            and that are not in the given list
 * \param listOfDiversToKeep : list of divers not to remove from the dive with id "diveId"
 * \param diveId : the dive aimed
 * \param db
 * \param table
 * \return
 */
bool removeFromDBNotIn(const QVector<data::Dive::MinimalDiver>& listOfDiversToKeep,int diveId,QSqlDatabase db,const QString& table);

} // namespace db

#endif // DB_DIVE_H
