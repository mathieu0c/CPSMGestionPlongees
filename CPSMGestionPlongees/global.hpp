#ifndef GLOBAL_GLOBAL_HPP
#define GLOBAL_GLOBAL_HPP

#include <QString>
#include <QObject>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#pragma clang diagnostic ignored "-Wstring-plus-int"

//#if defined(_MSC_VER)
//    #define DISABLE_WARNING_PUSH           __pragma(warning( push ))
//    #define DISABLE_WARNING_POP            __pragma(warning( pop ))
//    #define DISABLE_WARNING(warningNumber) __pragma(warning( disable : warningNumber ))

//    #define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER    DISABLE_WARNING(4100)
//    #define DISABLE_WARNING_UNREFERENCED_FUNCTION            DISABLE_WARNING(4505)
//    // other warnings you want to deactivate...

//#elif defined(__GNUC__) || defined(__clang__)
//    #define DO_PRAGMA(X) _Pragma(#X)
//    #define DISABLE_WARNING_PUSH           DO_PRAGMA(GCC diagnostic push)
//    #define DISABLE_WARNING_POP            DO_PRAGMA(GCC diagnostic pop)
//    #define DISABLE_WARNING(warningName)   DO_PRAGMA(GCC diagnostic ignored #warningName)

//    #define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER       DISABLE_WARNING(-Wunused-parameter)
//    #define DISABLE_WARNING_UNREFERENCED_FUNCTION               DISABLE_WARNING(-Wunused-function)
//    #define DISABLE_WARNING_STRING_PLUS_INT                     DISABLE_WARNING(-Wstring-plus-int)
//   // other warnings you want to deactivate...

//#else
//    #define DISABLE_WARNING_PUSH
//    #define DISABLE_WARNING_POP
//    #define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER
//    #define DISABLE_WARNING_UNREFERENCED_FUNCTION
//    // other warnings you want to deactivate...

//#endif

namespace
{
//the use of this macro may generate a warning because we're adding an int
//to a char*
#define __FILENAME_PRIVATE__ (__FILE__ + SOURCE_PATH_SIZE)
//constexpr auto __FILENAME_PRIVATE__{__FILE__ + SOURCE_PATH_SIZE};
}


namespace global
{

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
bool matchRegex(const QString& str,const QString& regex)
{
    QRegularExpression re{regex};
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
