#include "Address.h"

#include <QDebug>

namespace data
{

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

}//namespace data
