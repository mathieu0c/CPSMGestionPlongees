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

Dive readDiveFromDB(int id, QSqlDatabase db, QString table);

void removeDiversFromDive(Dive& dive,QVector<int> idList);
DiveType getDiveTypeForDiver(const Dive& dive,int diverId);
bool setDiveTypeForDiver(Dive& dive, int diverId, DiveType type);

QString to_string(DiveType diveType);
DiveType diveTypefrom_string(const QString& diveType);

QDebug operator<<(QDebug debug, const Dive& m);
QDebug operator<<(QDebug debug, const Dive::MinimalDiver& d);

} // namespace info

#endif // INFO_DIVE_H
