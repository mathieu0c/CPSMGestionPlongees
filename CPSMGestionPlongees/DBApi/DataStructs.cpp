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

bool operator==(const data::Address& a, const data::Address& b)
{
    return  a.id == b.id &&
            a.city == b.city &&
            a.postalCode == b.postalCode &&
            a.address == b.address;
}

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

QDebug operator<<(QDebug debug, const data::DiveMember& m)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "DiveMember{\ndiveId : " << m.diveId << "\n";
    debug.nospace() << "diverId : " << m.diverId << "\n";
    debug.nospace() << "Dive type : " << to_string(m.type) << "\n";
    debug.nospace() << "};";

    return debug;
}

void removeDiversFromDive(data::Dive& dive,QVector<int> idList)
{
    dive.diver.erase(std::remove_if(dive.diver.begin(),dive.diver.end(),[&](data::DiveMember& diver){
        return idList.contains(diver.diverId);
    }),dive.diver.end());
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
    for(const auto& e : m.diver)
    {
        debug.nospace() <<"    " << e << "\n";
    }
    debug.nospace() << "};";

    return debug;
}


//###########################################
//############               ################
//#########     DiverLevel      #############
//############               ################
//###########################################

QDebug operator<<(QDebug debug, const data::DiverLevel& m)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "DiverLevel{\nid : " << m.id << "\n";
    debug.nospace() << "Level name : " << m.level << "\n";
    debug.nospace() << "Sort value : " << m.sortValue << "\n";
    debug.nospace() << "};";

    return debug;
}

QString to_string(const data::DiverLevel& level) {
    QString str{};
    QTextStream{&str} << "{" << level.id << "," <<
                level.level << "," <<
                level.sortValue << "}";
    return str;
}


//###########################################
//############               ################
//#########       Diver         #############
//############               ################
//###########################################

QDebug operator<<(QDebug debug, const data::Diver& m)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "Diver{\nid : " << m.id << "\n";
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
