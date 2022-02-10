#include "DBApi/DataStructs.hpp"
#include "../global.hpp"

#include <QDebug>

namespace db{
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



}//namespace data
}//namespace db
