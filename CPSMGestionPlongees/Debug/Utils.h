#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#include <QSqlTableModel>
#include <QSqlDatabase>

#include <QVariant>
#include <QVector>

#include <QString>
#include <QStringList>

namespace debug
{

QSqlTableModel* sqlModel(QSqlDatabase db,const QString& table, const QStringList& headers,QObject* parent = nullptr);
void sqlModel(QSqlTableModel* model,const QString& table,const QStringList& headers);

void printDBQuery(const QVector<QVector<QVariant>>& queryResult);

}


#endif // DEBUG_UTILS_H
