#ifndef DBSAVER_HPP
#define DBSAVER_HPP

/*!
 * This file contains every utility function used to create databse saves
 *
*/

#include "../global.hpp"
#include <QDate>
#include <QJsonObject>
#include <any>
#include "GenericSettings/GenericSettings.hpp"

namespace db{

struct DBSaverInfo
{
    QString saveFolder{global::settings_dataDir+"Save/"};

    int frequentSaveCount{7};//how many save we should keep
    int spacedSaveCount{3};//how many save we should keep

    int frequentSaveInterval{2};//how many days between two saves
    int spacedSaveInterval{30};//how many days between two saves


    QString frequentSaveFolder()const{
        return saveFolder+"/"+m_frequentSaveFolder+"/";
    }
    QString spacedSaveFolder()const{
        return saveFolder+"/"+m_spacedSaveFolder+"/";
    }

private:
    const QString m_frequentSaveFolder{"Frequent"};
    const QString m_spacedSaveFolder{"Spaced"};
};

//-------------------------------------------------------------------------------------------------

bool refreshSave(DBSaverInfo& info);


//-------------------------------------------------------------------------------------------------

QJsonObject dbSaverInfoToJson(gens::SetPtr ptr);
std::any dbSaverInfoFromJson(const QJsonObject&);


}//namespace db


namespace{

inline
QDate retrieveDateFromFilename(const QFileInfo& file)
{
    return QDate::fromString(file.completeBaseName(),global::format_date);
}

bool p_refreshSave(const QString& targetSaveFolder,int saveInterval,int saveCount);

}//namespace


#endif // DBSAVER_HPP
