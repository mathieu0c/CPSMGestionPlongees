#include "User.h"

#include <QDebug>

namespace data
{

QDebug operator<<(QDebug debug, const User& u)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "User{\nLogin : " << u.login << "\n";
    debug.nospace() << "Hashed password : " << u.hashedPassword << "\n";
    debug.nospace() << "Valid user : " << u.valid << "\n";
    debug.nospace() << "Right level : " << u.rightLevel << "\n};";

    return debug;
}


}
