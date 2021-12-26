#ifndef INFO_GENERIC_HPP
#define INFO_GENERIC_HPP

/*!
 * This file contains generic functions common to info::Types
*/

#include <QSqlDatabase>

namespace info
{

/*  This function modify an existing object in the DB or create it if not existing
 *  It requires the type T to :
 *      - Have an "id" attribute
 *      - Have a function defined : "exists(T object,QSqlDatabase db, const QString& table)"
 *      - Have a function defined : "addToDB(T& object,QSqlDatabase db, const QString& table)"
 *      - Have a function defined : "updateDB(T& object,QSqlDatabase db, const QString& table)"
 *  This function may alter object "id" and subobjects "id" as well
 */

template<typename T>
int storeInDB(T &object, QSqlDatabase db, const QString &table)
{
    auto id{exists(object,db,table)};
    if(id == -1)//if the object doesn't exist
    {
        id = addToDB(object,db,table);
        object.id = id;
        return id;
    }
    else
    {
        if(object.id == -1)
            object.id = id;

        if(!updateDB(object,db,table))
            return -1;
        return id;
    }

    return -1;
}

}//namespace info

#endif // INFO_GENERIC_HPP
