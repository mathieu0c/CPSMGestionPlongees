#ifndef DIVER_H
#define DIVER_H

#include "Info/global.hpp"
#include "Info/Address.h"

#include <QSqlDatabase>

#include <QString>
#include <QDate>
#include <QVariant>

#include <QDebug>

namespace info
{

struct Diver
{
    int id{-1};
    QString firstName{};
    QString lastName{};

    QDate birthDate{QDate(2000,1,1)};

    QString email{};
    QString phoneNumber{};
    Address address{};

    QString licenseNumber{};
    QDate certifDate{QDate::currentDate()};
    int diverLevelId{1};

    bool member{false};

    int diveCount{0};
    int paidDives{0};

    bool gear_regulator{false};
    bool gear_suit{false};
    bool gear_computer{false};
    bool gear_jacket{false};

    int sold(){return paidDives-diveCount;}
};


/*------------------------*/

QDebug operator<<(QDebug debug, const Diver& m);

//return the id of the element added. -1 if failed
int addToDB(Diver &diver, QSqlDatabase db, QString table);

//This function may change diver.address.id
bool updateDB(Diver &diver, QSqlDatabase db, QString table, bool checkExistence = false);

Diver readDiverFromDB(int id, QSqlDatabase db, QString table);

//search if the address already exists and return id if true, -1 otherwise
int exists(const Diver& a, QSqlDatabase db, const QString &table);

//alter data if existing
//add data if not
//return data id
//this function may change diver.id and diver.address.id
int storeInDB(Diver& diver, QSqlDatabase db, const QString &table);

void removeAllFromDiver(int id, QSqlDatabase db, const QString& table);

QString to_string(const Diver& diver);
}

#endif // DIVER_H
