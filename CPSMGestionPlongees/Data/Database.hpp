#ifndef DB_DATABASE_HPP
#define DB_DATABASE_HPP

#include <QStringList>

#include <QSqlDatabase>

namespace db
{

extern bool enableDebug/*{false}*/;//should print debug message or not

//----------------------------------------------------------------

static QString c_extension_DB{".db"};//extension used to read/write files

bool openLocal(const QString &fileName, const QString &connectionName = "");

int countField(QSqlDatabase& db,const QString& table,const QString& field,QVariant value);

QVector<QVector<QVariant>> querySelect(QSqlDatabase& db,QString request,const QStringList& argList,const QVector<QVariant>& valList);

//return the number of row matching the query result
/*!
 * \brief queryCount : return the count of occurences for a standard "SELECT X FROM Y...". Note that this function expect
 * this pattern to be followed.
 * \param db
 * \param request
 * \param argList
 * \param valList
 * \return
 */
int queryCount(QSqlDatabase& db,QString request,const QStringList& argList,const QVector<QVariant>& valList);

bool queryExist(QSqlDatabase& db,QString request,const QStringList& argList,const QVector<QVariant>& valList);

//----------------- Local

bool createDB(QSqlDatabase db = QSqlDatabase::database());

bool initDB(QSqlDatabase db = QSqlDatabase::database());

QStringList getDiverLevels(QSqlDatabase db = QSqlDatabase::database());
QStringList getDiveSites(QSqlDatabase db = QSqlDatabase::database());

}

#endif // DATABASE_HPP
