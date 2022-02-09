#ifndef USER_H
#define USER_H

#include <QString>

#include <QDebug>

namespace data
{

struct User
{
    QString login{};
    QString hashedPassword{};
    bool valid{false};

    int rightLevel{15};//0 min -> max level
};

QDebug operator<<(QDebug debug, const User& u);

}//namespace data

#endif // USER_H
