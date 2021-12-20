#ifndef USER_H
#define USER_H

#include <QString>

#include <QDebug>

namespace info
{

struct User
{
    QString login{};
    QString hashedPassword{};
    bool valid{false};

    int rightLevel{15};//0 min -> max level
};

QDebug operator<<(QDebug debug, const User& u);

}//namespace info

#endif // USER_H
