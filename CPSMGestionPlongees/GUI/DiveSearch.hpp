#ifndef DIVESEARCH_H
#define DIVESEARCH_H

#include <QWidget>
#include <QSqlDatabase>
#include <QAbstractItemView>

#include "../global.hpp"

namespace Ui {
class DiveSearch;
}

namespace gui
{

class DiveSearch : public QWidget
{
    Q_OBJECT

public:
    explicit DiveSearch(QWidget *parent = nullptr);
    ~DiveSearch();

    void setDBName(QString name){
        m_dbName = std::move(name);
    }

    void setHiddenButton(bool hide);

    void setSelectionMode(QAbstractItemView::SelectionMode mode);

    void setFilter(QString newFilter,const QStringList& argList = {},QVector<QVariant> filterValues = {});

    void setSelectionColumns(QString sqlColumns,QStringList columnsNames = {}){
        m_sql_divesColumns = std::move(sqlColumns);
        m_sql_divesColumns += QString{",%0.id"}.arg(global::table_dives);
        m_gui_divesColumnsNames = std::move(columnsNames);
        m_initGui = false;//we need to reset GUI config
    }

    int getRowCount();

    void refreshDivesList();

    QVector<int> getSelectedDivesId();
    QVector<int> getDisplayedDivesId();

signals:
    void divesSelected(QVector<int> idList);
    void askRefreshDivesList();//this signal is sent at the beginning of the function refreshDiverList()

private slots:
    void on_tv_dives_doubleClicked(const QModelIndex &index);

private:
    QSqlDatabase db(){
        return QSqlDatabase::database(m_dbName,false);
    }


private:
    Ui::DiveSearch *ui;

    QString m_filter{};//Additionnal filter used to display diver list
    QVector<QVariant> m_filterValues{};//Filter values binded to query

    /*! Columns to be selected when the table widget is refreshed
     *  Default value is set in the constructor
     */
    QString m_sql_divesColumns{/*"date,DivingSites.name,COUNT(DivesMembers.diverId)"*/};
    /*! Columns name to be displayed when the table widget is refreshed
     *  Default value is set in the constructor
     */
    QStringList m_gui_divesColumnsNames{/*"Date","Site","Nombre de plongeurs"*/};

    QString m_dbName{QSqlDatabase::defaultConnection};

    bool m_initGui{false};
};

}//namespace gui

#endif // DIVESEARCH_H
