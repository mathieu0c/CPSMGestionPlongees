#include "SettingsManager.hpp"

#include <QDebug>




namespace gens {

SettingsManager::SettingsManager(QObject* parent) : QObject(parent),m_setList()
{
}

SettingsManager::~SettingsManager()
{

}

bool SettingsManager::addSetting(const BaseSetting& set)
{
    return instance()->p_addSetting(set);
}

#ifndef COMPILED_FOR_RELEASE
void SettingsManager::debug()
{
    for(const auto& [key,value] : instance()->m_setList)
    {
        qDebug() << "*******************";
        qDebug() << "Key : " << key << " for : " << value.content.get()->type().name();
        qDebug() << "toJson : " << value.toJson(value.content);
    }
}
#endif

// private members

bool SettingsManager::p_addSetting(const BaseSetting& set)
{
    if(m_setList.contains(set.name))
    {
        throw std::runtime_error{__func__+std::string{"This setting name already exists <"}+set.name.toStdString()+">"};
        return false;
    }

    m_setList[set.name] = set;

    return true;
}

QJsonObject SettingsManager::p_generateJson()
{
    QJsonObject main;
    for(const auto& [key,value] : m_setList){
        auto tmpJson{value.toJson(value.content)};
        main.insert(key,tmpJson);
    }
    return main;
}

void SettingsManager::p_parseJson(const QJsonObject& root)
{
    for(auto& [key,value] : m_setList){
        if(!root.contains(key)){
            qCritical() << __func__ << " : could not find setting sor the key : " << key;
            continue;
        }
        *(value.content.get()) = value.fromJson(root.value(key).toObject());
    }
}

} // namespace gens
