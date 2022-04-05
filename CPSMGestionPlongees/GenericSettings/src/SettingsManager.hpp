#ifndef GENS_SETTINGSMANAGER_HPP
#define GENS_SETTINGSMANAGER_HPP

#include <QObject>
#include <QString>

#include "BaseSetting.hpp"
#include <unordered_map>

#include "JSONTools.hpp"
#include <QJsonObject>
#include <QJsonValue>

#include <QFileInfo>

namespace gens {

template<typename T>
T* settCast(const std::shared_ptr<std::any>& ptr){
    return std::any_cast<T>(ptr.get());
}


//Singleton

class SettingsManager : public QObject
{
Q_OBJECT

private:
    bool p_addSetting(const BaseSetting& set);

    template<typename T>
    T* p_at(const QString& key);

public:
    static SettingsManager* instance()
    {
        static SettingsManager m_instance{};
        return &m_instance;
    }
    static SettingsManager* i(){return instance();}


    static bool addSetting(const BaseSetting& set);
    template<typename T>//function for base types like int, bool, etc...
    static bool addSetting(const QString& key,const T& val);

    template<typename T>//function for complex types
    static bool addSetting(const QString& key,const T& val,
                           ToJsonFunction toJson,FromJsonFunction fromJson);

    template<typename T>
    static T* at(const QString& key);
    template<typename T>
    static T& ref(const QString& key);



    static QJsonObject json(){
        return i()->p_generateJson();
    }

    static bool saveAs(const QString& fileName)
    {
        return utils::save(i()->p_generateJson(),fileName);
    }
    static bool read(const QString& fileName,bool skipNotExisting = true){
        if(!QFileInfo::exists(fileName))
        {
            if(skipNotExisting)
            {
                return true;
            }
            else
            {
                return false;
            }
        }


        auto docOpt{utils::read(fileName)};
        if(!docOpt){
            qCritical() << QString::fromStdString({"SettingsManager.hpp : "+std::string{__LINE__}+" : "+
                                     std::string{"Cannot find settings file "}+fileName.toStdString()});
            return false;
        }
        auto docObj{docOpt.value().object()};
        i()->p_parseJson(docObj);
        return true;
    }

#ifndef COMPILED_FOR_RELEASE
    static void debug();
#endif

private:
    QJsonObject p_generateJson();
    void p_parseJson(const QJsonObject& root);

private:
    SettingsManager(QObject* parent=nullptr);
    ~SettingsManager();
    SettingsManager(const SettingsManager&)= delete;
    SettingsManager& operator=(const SettingsManager&)= delete;

    std::unordered_map<QString,BaseSetting> m_setList;
};

template<typename T>
T* SettingsManager::p_at(const QString& key)
{
    try
    {
        return settCast<T>(m_setList.at(key).content);
    }
    catch(const std::out_of_range& e)
    {
        qCritical() << "SettingsManager"<<__func__ << " : Not setting found with key <"<<key<<">";
        return nullptr;
    }
}


template<typename T>
bool SettingsManager::addSetting(const QString& key,const T& val)
{
    gens::BaseSetting bs{
        .name=key,
        .content=std::make_shared<std::any>(val),
        .toJson=[&](SetPtr ptr)->QJsonObject{
            auto v{settCast<T>(ptr)};
            QJsonObject rval{};
            rval.insert("val",*v);
            return rval;
        },
        .fromJson=[&](const QJsonObject& obj)->std::any{
            return std::any(obj.value("val").toVariant().value<T>());
        }
    };
    return addSetting(bs);
}

template<typename T>
bool SettingsManager::addSetting(const QString& key,const T& val,
                           ToJsonFunction toJson,FromJsonFunction fromJson)
{
    gens::BaseSetting bs{
        .name=key,
        .content=std::make_shared<std::any>(val),
        .toJson=toJson,
        .fromJson=fromJson
    };
    return addSetting(bs);
}

template<typename T>
T* SettingsManager::at(const QString& key){
    return i()->p_at<T>(key);
}
template<typename T>
T& SettingsManager::ref(const QString& key)
{
    return *(at<T>(key));
}


} // namespace gens

#endif // GENS_SETTINGSMANAGER_HPP
