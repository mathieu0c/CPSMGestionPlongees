#include "DBApi/DataStructs.hpp"
#include "../global.hpp"

#include <QDebug>

//namespace db{
namespace data
{

//###########################################
//############               ################
//#########       Address       #############
//############               ################
//###########################################

QDebug operator<<(QDebug debug, const Address& a)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "{\nid : "<<a.id<<"\n";
    debug.nospace() << a.address<<"\n";
    debug.nospace() << a.postalCode<<"\n";
    debug.nospace() << a.city<<" };";

    return debug;
}

QString to_string(const Address& address)
{
    QString str{};
    QTextStream{&str} << "{" << address.id << "," <<
                address.address << "," <<
                address.postalCode << "," <<
                address.city << "}";
    return str;
}




//###########################################
//############               ################
//#########        Dive         #############
//############               ################
//###########################################

void removeDiversFromDive(data::Dive& dive,QVector<int> idList)
{
    dive.divers.erase(std::remove_if(dive.divers.begin(),dive.divers.end(),[&](data::Dive::MinimalDiver& diver){
        return idList.contains(diver.id);
    }),dive.divers.end());
}

data::DiveType getDiveTypeForDiver(const data::Dive& dive,int diverId)
{
    auto itDiver{std::find_if(dive.divers.begin(),dive.divers.end(),[&](const data::Dive::MinimalDiver& e){
            return e.id == diverId;
        })};
    if(itDiver == dive.divers.end())
    {
        return data::DiveType::undefined;
    }

    return (*itDiver).type;
}

bool setDiveTypeForDiver(data::Dive& dive,int diverId,data::DiveType type)
{
    auto itDiver{std::find_if(dive.divers.begin(),dive.divers.end(),[&](const data::Dive::MinimalDiver& e){
            return e.id == diverId;
        })};
    if(itDiver == dive.divers.end())
    {
        return false;
    }
    (*itDiver).type = type;

    return true;
}

QString to_string(data::DiveType diveType)
{
    switch (diveType)
    {
    case data::DiveType::exploration:
        return "Explo";
    case data::DiveType::technical:
        return "Tech";
    default:
        return "-";
    }
}

data::DiveType diveTypefrom_string(const QString& diveType)
{
    if(diveType == "Explo")
        return data::DiveType::exploration;
    else if(diveType == "Tech")
        return data::DiveType::technical;

    return data::DiveType::undefined;
}

QDebug operator<<(QDebug debug, const data::Dive& m)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "Dive{\nid : " << m.id << "\n";
    debug.nospace() << "Date : " << m.date.toString(global::format_date) << "\n";
    debug.nospace() << "Time : " << m.time.toString(global::format_time) << "\n";
    debug.nospace() << "diveSiteId : " << m.diveSiteId << "\n";
    debug.nospace() << "Divers :" << "\n";
    for(const auto& e : m.divers)
    {
        debug << QString("    Id : %0    |   DiveType : %1").arg(e.id).arg(e.type) << "\n";
    }
    debug.nospace() << "};";

    return debug;
}


//###########################################
//############               ################
//#########       Diver         #############
//############               ################
//###########################################

QDebug operator<<(QDebug debug, const data::Diver& m)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "Member{\nid : " << m.id << "\n";
    debug.nospace() << "First name : " << m.firstName << "\n";
    debug.nospace() << "Last name : " << m.lastName << "\n";
    debug.nospace() << "Birth date : " << m.birthDate << "\n";
    debug.nospace() << "Email : " << m.email << "\n";
    debug.nospace() << "Phone : " << m.phoneNumber << "\n";
    debug.nospace() << "Address : " << m.address << "\n";
    debug.nospace() << "License number : " << m.licenseNumber << "\n";
    debug.nospace() << "Certificate date : " << m.certifDate << "\n";
    debug.nospace() << "Level id : " << m.diverLevelId << "\n";
    debug.nospace() << "Member : " << m.member << "\n";
    debug.nospace() << "Dive count : " << m.diveCount << "\n";
    debug.nospace() << "Paid dives : " << m.paidDives << "\n";
    debug.nospace() << "Gear :\n";
    debug.nospace() << "\t- Regulator : " << m.gear_regulator << "\n";
    debug.nospace() << "\t- Suit : " << m.gear_suit << "\n";
    debug.nospace() << "\t- Computer : " << m.gear_computer << "\n";
    debug.nospace() << "\t- Jacket : " << m.gear_jacket << "\n";
    debug.nospace() << "};";

    return debug;
}

QString to_string(const data::Diver& diver)
{
    QString str{};
    QTextStream{&str} << "{" << diver.id << "," <<
                diver.firstName << "," <<
                diver.lastName << "," <<
                diver.birthDate.toString(global::format_date) << "," <<
                diver.email << "," <<
                diver.phoneNumber << "," <<
                to_string(diver.address) << "," <<
                diver.licenseNumber << "," <<
                diver.certifDate.toString(global::format_date) << "," <<
                diver.diverLevelId << "," <<
                diver.member << "," <<
                diver.diveCount << "," <<
                diver.paidDives << "," <<
                diver.gear_regulator << "," <<
                diver.gear_suit << "," <<
                diver.gear_computer << "," <<
                diver.gear_jacket << "}";
    return str;
}

}//namespace data
//}//namespace db
