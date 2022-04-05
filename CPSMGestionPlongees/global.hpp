#ifndef GLOBAL_GLOBAL_HPP
#define GLOBAL_GLOBAL_HPP

#include <QString>
#include <QObject>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include <QStandardPaths>

#pragma clang diagnostic ignored "-Wstring-plus-int"

namespace
{
#define __FILENAME_PRIVATE__ (__FILE__ + SOURCE_PATH_SIZE)
}


namespace global
{

//-- soft settings

static const auto settings_dataDir{QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/"+QString{PROJECT_NAME}+"/"};
static const QString settings_confFile{settings_dataDir+"settings.conf"};

//sk stands for settingsKey
static const auto sk_dbPath{"DBPath"};

//-- DB

static const QString table_divers{"Divers"};
static const QString table_diversAddresses{"DiversAddresses"};
static const QString table_divingSites{"DivingSites"};
static const QString table_diverLevel{"DiverLevel"};
static const QString table_dives{"Dives"};
static const QString table_divesMembers{"DivesMembers"};

//-- format

static const QString format_date{"yyyy-MM-dd"};
static const QString format_time{"HH:mm:ss"};

#ifndef __LINE__
#error "Line macro used for debugging purpose is not defined"
#endif

#ifndef __FILE__
#error "File macro used for debugging purpose is not defined"
#else
#define _LINE_ QString::number(__LINE__)
#endif


#define __CURRENT_PLACE__ QString{"%0 : <%1> : l.%2"}.arg(__FILENAME_PRIVATE__,__func__,_LINE_)


//------------- Tools

namespace tools
{
template<typename T,typename UnaryFunction>
void applyToChildren(QObject* parent,UnaryFunction lambda)
{
    auto objList{parent->findChildren<T>()};
    for(const auto& e : objList)
    {
        lambda(e);
    }
}

inline
bool matchRegex(const QString& str,const QString& regex,bool caseInsensitive = true)
{
    QRegularExpression re{regex};
    if(caseInsensitive)
        re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    auto ans{re.match(str,0,QRegularExpression::MatchType::NormalMatch,
                      QRegularExpression::NoMatchOption)};
    return ans.hasMatch();
}


}//namespace tools

}//namespace global

namespace db
{
extern bool enableDebug/*{false}*/;
}

namespace data
{
extern bool enableDebug/*{false}*/;
}//namespace data

#endif // GLOBAL_HPP
