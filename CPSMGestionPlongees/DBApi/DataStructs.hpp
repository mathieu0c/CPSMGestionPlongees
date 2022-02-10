#ifndef DB_DATASTRUCTS_HPP
#define DB_DATASTRUCTS_HPP

#include <QDebug>
#include <QString>
#include <QDate>
#include <QTime>

namespace db
{

namespace data
{

//###########################################
//############               ################
//#########       Address       #############
//############               ################
//###########################################

struct Address
{
    int id{-1};
    QString address{};
    QString postalCode{};
    QString city{};
};

QDebug operator<<(QDebug debug, const Address& a);

QString to_string(const Address& address);



//###########################################
//############               ################
//#########        Dive         #############
//############               ################
//###########################################


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
    QTime time{};
    int diveSiteId{1};
    QVector<MinimalDiver> divers{};//diver id and type

};

//###########################################
//############               ################
//#########       Diver         #############
//############               ################
//###########################################

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


}//namespace data

}//namespace db

#endif // DB_DATASTRUCTS_HPP
