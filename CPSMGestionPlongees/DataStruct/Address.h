#ifndef INFO_ADDRESS_H
#define INFO_ADDRESS_H

#include <QString>

#include <QDebug>

namespace data
{

struct Address
{
    int id{-1};
    QString address{};
    QString postalCode{};
    QString city{};
};

QDebug operator<<(QDebug debug, const Address& a);

QString to_string(const Address& address);

}//namespace data

#endif // INFO_ADDRESS_H
