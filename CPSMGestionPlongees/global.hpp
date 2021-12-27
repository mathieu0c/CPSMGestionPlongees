#ifndef GLOBAL_GLOBAL_HPP
#define GLOBAL_GLOBAL_HPP

#include <QString>

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

#ifndef __LINE__
#error "Line macro used for debugging purpose is not defined"
#endif

#ifndef __FILE__
#error "File macro used for debugging purpose is not defined"
#else
#define _LINE_ QString::number(__LINE__)
#endif

}

#endif // GLOBAL_HPP
