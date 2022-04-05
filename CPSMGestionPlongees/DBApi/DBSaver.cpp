#include "DBSaver.hpp"

#include "../global.hpp"
#include <QDate>
#include <QJsonObject>
#include <any>
#include "GenericSettings/GenericSettings.hpp"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QFile>

#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include <map>

namespace{

bool p_refreshSave(const QString& targetSaveFolder,int saveInterval,int saveCount)
{
    const auto& usedDbPath{gens::SettingsManager::ref<QString>(global::sk_dbPath)};
    QDate currentDate{QDate::currentDate()};
    const QString todaySaveFileName{currentDate.toString(global::format_date)+"."
                                    +QFileInfo{usedDbPath}.suffix()};

    if(db::enableDebug)
    {
        qDebug() << "------------- " << __CURRENT_PLACE__ << " --------------";
    }

    if(!QFileInfo::exists(targetSaveFolder))
    {
        if(!QDir::root().mkpath(targetSaveFolder))
        {
            throw std::runtime_error{__CURRENT_PLACE__.toStdString() + " : Cannot create appData folder : <" + targetSaveFolder.toStdString() + ">"};
        }
    }

    std::map<QDate,QString> saveMap{};
    QDir freqDir{targetSaveFolder};
    QDirIterator freqIt{freqDir};
    while(freqIt.hasNext())
    {
        freqIt.next();
        auto inf{freqIt.fileInfo()};
        QDate saveDate{retrieveDateFromFilename(inf)};
        if(!saveDate.isValid())
            continue;
        saveMap[saveDate] = inf.fileName();
    }

    auto lastSaveDate{((size(saveMap) == 0)?QDate{2000,01,01}:rbegin(saveMap)->first)};

    bool success{true};

    if(lastSaveDate.addDays(saveInterval) < currentDate)//if the last save is outdated
    {
        QString saveFilePath{targetSaveFolder+todaySaveFileName};
        success=QFile::copy(usedDbPath,saveFilePath);//copy the DB
        if(db::enableDebug)
        {
            qDebug() << "Copying :"<< usedDbPath;
            qDebug() << "to :" << saveFilePath;
        }
        if(success)
        {
            saveMap[currentDate] = saveFilePath;
        }
    }


    auto saveCountDiff{size(saveMap) - saveCount};
    if(size(saveMap) < saveCount)
        saveCountDiff = 0;
    qDebug() << saveCountDiff;
    if(saveCountDiff > 0)//if we have too many saves
    {
        QStringList filesToRemove{};
        filesToRemove.reserve(saveCountDiff);
        int toRemoveCount{};

        for(const auto&[key,value] : saveMap)
        {
            if((++toRemoveCount) > saveCountDiff)
                break;
            filesToRemove.append(value);
        }

        for(const auto& target : filesToRemove)
        {
            auto tmpSuccess{QFile::remove(targetSaveFolder+target)};
            if(db::enableDebug)
                qDebug() <<"Removing outdated :" <<target << " : " << tmpSuccess;
        }
    }

    return success;//success of copying only. It is not a big matter if we can't delete old versions
}

}

namespace db {

//-------------------------------------------------------------------------------------------------

bool refreshSave(DBSaverInfo& info)
{
    return p_refreshSave(info.frequentSaveFolder(),info.frequentSaveInterval,info.frequentSaveCount)&&
            p_refreshSave(info.spacedSaveFolder(),info.spacedSaveInterval,info.spacedSaveCount);
}


//-------------------------------------------------------------------------------------------------

QJsonObject dbSaverInfoToJson(gens::SetPtr ptr)
{
    using namespace gens;
    auto v{settCast<DBSaverInfo>(ptr)};
    QJsonObject rval{};
    rval.insert("saveFolder",v->saveFolder);

    rval.insert("frequentSaveCount",v->frequentSaveCount);
    rval.insert("spacedSaveCount",v->spacedSaveCount);

    rval.insert("frequentSaveInterval",v->frequentSaveInterval);
    rval.insert("spacedSaveInterval",v->spacedSaveInterval);
    return rval;
}

std::any dbSaverInfoFromJson(const QJsonObject& obj)
{
//return std::any(obj.value("val").toVariant().value<T>());
    DBSaverInfo out{};
    out.saveFolder = obj.value("saveFolder").toString();

    out.frequentSaveCount = obj.value("frequentSaveCount").toInt(-1);
    out.spacedSaveCount = obj.value("spacedSaveCount").toInt(-1);

    out.frequentSaveInterval = obj.value("frequentSaveInterval").toInt(-1);
    out.spacedSaveInterval = obj.value("spacedSaveInterval").toInt(-1);

    return out;
}


} // namespace db
