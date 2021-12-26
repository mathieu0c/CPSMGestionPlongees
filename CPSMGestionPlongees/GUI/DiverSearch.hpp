#ifndef GUI_DIVERSEARCH_HPP
#define GUI_DIVERSEARCH_HPP

#include "../global.hpp"

#include <QVector>

#include <QWidget>
#include <QAbstractItemView>
#include <QTableView>

#include <QSqlDatabase>

namespace Ui {
class DiverSearch;
}

namespace gui
{

class DiverSearch : public QWidget
{
    Q_OBJECT

public:
    explicit DiverSearch(QWidget *parent = nullptr);
    ~DiverSearch();

    void setDBName(QString name){
        m_dbName = std::move(name);
    }

    void setHiddenButton(bool hide);

    void setSelectionMode(QAbstractItemView::SelectionMode mode);

    void setFilter(QString newFilter,const QStringList& argList = {},QVector<QVariant> filterValues = {});
    void setFilterValues(QVector<QVariant> filterValues){
        m_filterValues = std::move(filterValues);
    }

    void setSqlJoins(QString sqlJoins,const QStringList& args)
    {
        m_sqlJoins = std::move(sqlJoins);

        for(const auto& e : args)//match argument list
        {
            m_sqlJoins = m_sqlJoins.arg(e);
        }
    }

//SELECT date,DivingSites.name,COUNT(DivesMembers.diverId) FROM Dives INNER JOIN DivingSites ON
//Dives.diveSiteId = DivingSites.id INNER JOIN DivesMembers on DivesMembers.diveId = Dives.id
    void setSelectionColumns(QString sqlColumns,QStringList columnsNames = {}){
        m_sql_diversColumns = std::move(sqlColumns);
        m_sql_diversColumns += QString{",%0.id"}.arg(global::table_divers);
        m_gui_diversColumnsNames = std::move(columnsNames);
        m_initGui = false;//we need to reset GUI config
    }

    int getRowCount()const;
    int getColumnCount()const;

    QTableView* table();//raw access to table view necessary to create and access ComboBox from DiveEdit
    QModelIndex indexAt(int x,int y)const;

    void refreshDiverList();

    QVector<int> getSelectedDiversId()const;
    QVector<int> getDisplayedDiversId()const;

    int getDiverIdAt(const QModelIndex& index) const;

    //-----


signals:
    void diversSelected(QVector<int> idList);
    void askRefreshDiverList();//this signal is sent at the beginning of the function refreshDiverList()
    void refreshedDiverList();//this signal is sent at the end of the function refreshDiverList()

private slots:
    void on_tv_divers_doubleClicked(const QModelIndex &index);

private:
    QSqlDatabase db() const{
        return QSqlDatabase::database(m_dbName,false);
    }

private:
    Ui::DiverSearch *ui;

    QString m_filter{};//Additionnal filter used to display diver list
    QVector<QVariant> m_filterValues{};//Filter values binded to query
    QString m_sqlJoins{};//joins added to global::table_divers
    //example : QString{" INNER JOIN %0 ON %1.id = %0.diverId"}.arg(global::table_divesMembers,global::table_divers)

    QString m_sql_diversColumns{/*"lastName,firstName,member,level,paidDives-diveCount"*/};
    QStringList m_gui_diversColumnsNames{/*"Nom de famille","Prénom","Membre","Niveau","Solde"*/};

    QString m_dbName{QSqlDatabase::defaultConnection};

    bool m_initGui{false};
};

}//namespace gui

#endif // GUI_DIVERSEARCH_HPP
