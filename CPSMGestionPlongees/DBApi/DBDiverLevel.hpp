#ifndef DB_DBDIVERLEVEL_HPP
#define DB_DBDIVERLEVEL_HPP

#include "DBApi/DataStructs.hpp"

#include <QSqlDatabase>

#include <QDebug>

namespace db {

data::DiverLevel extractDiverLevelFromQuery(const QSqlQuery& query);

data::DiverLevel readDiverLevelFromDB(int id, QSqlDatabase db, QString table);

QVector<data::DiverLevel> readDiverLevelLFromDB(QVector<int> idList, QSqlDatabase db, QString table);

//search if the address already exists and return id if true, -1 otherwise
int exists(const data::DiverLevel& l, QSqlDatabase db, const QString &table);

//alter data if existing
//add data if not
//return provided id
//Note : this function does not change the id stored in the object
int storeInDB(const data::DiverLevel &l, QSqlDatabase db, const QString &addressTable);

} // namespace db

#endif // DB_DBDIVERLEVEL_HPP
