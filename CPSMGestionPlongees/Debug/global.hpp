#ifndef DEBUG_GLOBAL_HPP
#define DEBUG_GLOBAL_HPP

#include <QString>
#include <QSqlQuery>

namespace debug
{

//static bool enableLogFile{false};
extern bool enableLogFile;
static QString c_extension_log{".log"};

/*!
 * \brief debugQuery allows to debug a simple query formulated with '?' for value binding
 * \param query debugged
 * \param fileAndLine : position from where this function was called. We advice to use __CURRENT_PLACE__ defined in /global.hpp
 */
void debugQuery(const QSqlQuery& query,const QString& fileAndLine);

}

#endif // DEBUG_GLOBAL_HPP
