#ifndef DB_DATASTRUCTS_HPP
#define DB_DATASTRUCTS_HPP

#include <QDebug>
#include <QString>
#include <QDate>
#include <QTime>

//namespace db
//{

namespace data
{

//###########################################
//############               ################
//#########      DivingSite     #############
//############               ################
//###########################################

struct DivingSite
{
    int id{-1};
    QString name;
};


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

bool operator==(const data::Address& a, const data::Address& b);
inline
bool operator!=(const data::Address& a, const data::Address& b){
    return !(a==b);
}

QDebug operator<<(QDebug debug, const Address& a);

QString to_string(const Address& address);



//###########################################
//############               ################
//#########     DiverLevel      #############
//############               ################
//###########################################

struct DiverLevel
{
    int id{-1};
    QString level{};
    int sortValue{0};
};

QDebug operator<<(QDebug debug, const data::DiverLevel& m);
QString to_string(const data::DiverLevel& diver);


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
    QDate registrationDate{QDate(2000,1,1)};

    int diveCount{0};
    int paidDives{0};

    bool gear_regulator{false};
    bool gear_suit{false};
    bool gear_computer{false};
    bool gear_jacket{false};

    int sold(){return paidDives-diveCount;}
};

QDebug operator<<(QDebug debug, const data::Diver& m);
QString to_string(const data::Diver& diver);


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

//###################################
//####               ################
//#      DiveMember     #############
//####               ################
//###################################

struct DiveMember
{
    int diveId;
    int diverId;
    DiveType type;
    Diver fullDiver;
};

QDebug operator<<(QDebug debug, const data::DiveMember& m);

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
    QVector<DiveMember> diver{};
    DivingSite site{};
};

void removeDiversFromDive(data::Dive& dive,QVector<int> idList);
data::DiveType getDiveTypeForDiver(const data::Dive& dive,int diverId);
bool setDiveTypeForDiver(data::Dive& dive, int diverId, data::DiveType type);

QString to_string(data::DiveType diveType);
data::DiveType diveTypefrom_string(const QString& diveType);

QDebug operator<<(QDebug debug, const data::Dive& m);


}//namespace data

//}//namespace db

#endif // DB_DATASTRUCTS_HPP
