#include "Utils.h"
namespace debug
{

QSqlTableModel* sqlModel(QSqlDatabase db,const QString& table, const QStringList& headers,QObject* parent)
{
    auto mod{new QSqlTableModel{parent,db}};

    mod->setTable(table);
    mod->setEditStrategy(QSqlTableModel::OnManualSubmit);
    mod->select();

    for(int i{}; i < headers.size();++i)
    {
        mod->setHeaderData(i,Qt::Horizontal,headers[i]);
    }

    return mod;
}

void sqlModel(QSqlTableModel* model,const QString& table,const QStringList& headers)
{
    model->setTable(table);
    model->select();

    for(int i{}; i < headers.size();++i)
    {
        model->setHeaderData(i,Qt::Horizontal,headers[i]);
    }
}

void printDBQuery(const QVector<QVector<QVariant>>& queryResult)
{
    for(const auto& line : queryResult)
    {
        qDebug() << "------------------------------";
        for(const auto& e : line)
            qDebug() << "\t" << e;
    }
}

}//namespace Debug
