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


//---------- Utils

/*!
 * \brief formatListForSQL : format a list like `{1,2,3}` in `"(1,2,3)"`
 * \param in : input list
 * \return formatted string
 */
template<typename T>
inline
QString formatListForSQL(const QVector<QVariant>& in)
{
    QString out{'('};
    for(const auto& e : in){
        out = out+QString("%0,").arg(e.value<T>());
    }
    if(out.endsWith(','))
        out.resize(out.size()-1);
    out += ")";
    return out;
}

template<typename T>
inline
QString formatListForSQL(const QVector<T>& in)
{
    QString out{'('};
    for(const auto& e : in){
        out = out+QString("%0,").arg(e);
    }
    if(out.endsWith(','))
        out.resize(out.size()-1);
    out += ")";
    return out;
}


/*!
 * \brief questionMarkList : create a list of questino mark for SQL requests.
 *              For example : `{1,3,6,9}` will produce `"(?,?,?,?)"`
 * \param in
 * \return
 */
template<typename T>
inline
QString questionMarkList(const QVector<T>& in)
{
    QString out{'('};
    for(const auto& e : in){
        out = out+"?,";
    }
    if(out.endsWith(','))
        out.resize(out.size()-1);
    out += ")";
    return out;
}

/*!
 * \brief prepRequestListFilter : prepare a list of elements for a SQL request
 * \param in : input list
 * \return a formatted string like (?,?,?,?) and a vector<QVariant> containing values to use in the sql req
*/
template<typename T>
inline
std::tuple<QString,QVector<QVariant>> prepRequestListFilter(const QVector<T>& in)
{
    auto ouStr{questionMarkList(in)};
    QVector<QVariant> outL(in.size());
    for(int i{}; i < outL.size();++i)
    {
        outL[i] = QVariant{in[i]};
    }
    return {ouStr,outL};
}


} // namespace db

#endif // DB_DBAPI_HPP
