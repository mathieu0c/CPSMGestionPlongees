#ifndef INFO_DIVE_H
#define INFO_DIVE_H

#include <QVector>
#include <QDate>
#include <tuple>

#include <QSqlDatabase>

#include <QDebug>

/*

SELECT Dives.date,DivingSites.name,DivesMembers.diveType,Divers.firstName,Divers.lastName FROM
((DIVES INNER JOIN DivesMembers ON DivesMembers.diveId=Dives.id)
INNER JOIN Divers ON DivesMembers.diverId=Divers.id)
INNER JOIN DivingSites ON Dives.diveSiteId=DivingSites.id

*/

namespace info {

enum DiveType{
    undefined=-1,
    exploration=0,
    technical=1
};

struct Dive
{
    struct MinimalDiver{
        int id{-1};
        DiveType type{};
    };

    int id{-1};
    QDate date{};
    int diveSiteId{1};
    QVector<MinimalDiver> divers{};//diver id and type

};

//return the id of the element added. -1 if failed
int addToDB(Dive &dive, QSqlDatabase db, QString table);

//This function may change diver.address.id
bool updateDB(Dive &dive, QSqlDatabase db, QString table, bool checkExistence = false);

Dive readDiveFromDB(int id, QSqlDatabase db, QString table);

//search if the diver already exists and return id if true, -1 otherwise
bool exists(const Dive& a, QSqlDatabase db, const QString &table);

//alter data if existing
//add data if not
//return data id
//this function may change diver.id and diver.address.id
//int storeInDB(Dive& diver, QSqlDatabase db, const QString &table);


void removeDiversFromDive(Dive& dive,QVector<int> idList);
DiveType getDiveTypeForDiver(const Dive& dive,int diverId);
bool setDiveTypeForDiver(Dive& dive, int diverId, DiveType type);

QString to_string(DiveType diveType);
DiveType diveTypefrom_string(const QString& diveType);

QDebug operator<<(QDebug debug, const Dive& m);

//-------------------------------------------------------------------------------------
//------------------------  MinimalDiver funcs  ---------------------------------------
//-------------------------------------------------------------------------------------

QDebug operator<<(QDebug debug, const Dive::MinimalDiver& d);

bool addToDB(const Dive::MinimalDiver& diver, int diveId, QSqlDatabase db, QString table);

bool updateDB(const Dive::MinimalDiver &diver, int diveId, QSqlDatabase db, QString table, bool checkExistence = false);

Dive::MinimalDiver readMinimalDiverFromDB(int diverId, int diveId, QSqlDatabase db, QString table);

//search if the minimalDive exist
bool exists(const Dive::MinimalDiver& diver, int diveId, QSqlDatabase db, const QString &table);

bool storeInDB(const Dive::MinimalDiver& diver,int diveId, QSqlDatabase db, const QString &table);

/*!
 * \brief removeFromDBNotIn : remove MinimalDivers participating to the dive with id=diveId from DB
 *                            and that are not in the given list
 * \param listOfDiversToKeep : list of divers not to remove from the dive with id "diveId"
 * \param diveId : the dive aimed
 * \param db
 * \param table
 * \return
 */
bool removeFromDBNotIn(const QVector<Dive::MinimalDiver>& listOfDiversToKeep,int diveId,QSqlDatabase db,const QString& table);

} // namespace info

#endif // INFO_DIVE_H
