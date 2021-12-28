#include "DiveSearch.hpp"
#include "ui_DiveSearch.h"

#include "GUI/constSettings.hpp"
#include "GUI/global.hpp"

#include "../global.hpp"

#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>

#include <QModelIndexList>
#include <QModelIndex>

#include <QDebug>

namespace gui
{

DiveSearch::DiveSearch(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiveSearch)
{
    ui->setupUi(this);

    ui->de_endDate->setDate(QDate::currentDate());

    //nullif(COUNT(DivesMembers.diverId),0)
    setSelectionColumns(QString{"date,%0.name,COUNT(%1.diverId)"}.arg(//nullif(COUNT(%1.id),0)"}.arg(
                            global::table_divingSites,
                            global::table_divesMembers
                            ),
                        {"Date","Site","Nombre de plongeurs"});

    ui->tv_dives->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

//    refreshDivesList();

    //refreshing diver list
    connect(ui->de_beginDate,&QDateEdit::dateChanged,[&](){refreshDivesList();});
    connect(ui->de_endDate,&QDateEdit::dateChanged,[&](){refreshDivesList();});
//    connect(ui->cb_search_firstName,&QCheckBox::stateChanged,[&](){refreshDiverList();});
//    connect(ui->cb_search_lastName,&QCheckBox::stateChanged,[&](){refreshDiverList();});
}

DiveSearch::~DiveSearch()
{
    delete ui;
}

void DiveSearch::setHiddenButton(bool hide)
{
    ui->buttonBox->setVisible(!hide);
}

void DiveSearch::setSelectionMode(QAbstractItemView::SelectionMode mode)
{
    ui->tv_dives->setSelectionMode(mode);
}

void DiveSearch::setFilter(QString newFilter,const QStringList& argList,QVector<QVariant> filterValues)
{
    m_filter = std::move(newFilter);
    m_filterValues = std::move(filterValues);

    for(const auto& e : argList)//match argument list
    {
        m_filter = m_filter.arg(e);
    }
}

int DiveSearch::getRowCount(){
    return ui->tv_dives->model()->rowCount();
}

void DiveSearch::refreshDivesList()
{
    if(ui->tv_dives->model() == nullptr) //if there is no model set in the view
    {
        ui->tv_dives->setModel(new QSqlQueryModel(this));//create it
    }

    if(!db().open())//if the db isn't open : quit function
        return;

    emit askRefreshDivesList();

    //SELECT date,DivingSites.name,COUNT(DivesMembers.diverId) FROM Dives INNER JOIN DivingSites
    //ON Dives.diveSiteId = DivingSites.id INNER JOIN DivesMembers on DivesMembers.diveId = Dives.id
    QString querStr{"SELECT %0 FROM %1 INNER JOIN %2 ON %1.diveSiteId = %2.id LEFT JOIN "
                    "%3 ON %3.diveId = %1.id"};
    querStr = querStr.arg(m_sql_divesColumns,
                          global::table_dives,
                          global::table_divingSites,
                          global::table_divesMembers);

    //auto querStrFirstName{querStr+" WHERE firstName LIKE ?||'%'"};
    //auto querStrLastName{querStr+" WHERE lastName LIKE ?||'%'"};
    QSqlQuery query{db()};

    bool hasFilter{!m_filter.isEmpty()};
    QString filter{};
    QVector<QVariant> valList{};

    addFilter("date >= date(?) AND date <= date(?)",{},{ui->de_beginDate->date().toString(global::format_date),
                                         ui->de_endDate->date().toString(global::format_date)});


    //if nothing is searched or there no search option selected
//    if(ui->le_search->text().isEmpty() || (!ui->cb_search_firstName->isChecked() && !ui->cb_search_lastName->isChecked()))
//    {
//        //no filter
//        //querStr += " ORDER BY lastName";
//    }
//    else
//    {
//        //querStr += " WHERE ";
//        if(ui->cb_search_firstName->isChecked() && ui->cb_search_lastName->isChecked())
//        {
//            filter = "firstName LIKE ?||'%' OR lastName LIKE ?||'%' "+QString(hasFilter?"AND ":"");
//            valList.append(ui->le_search->text());
//            valList.append(ui->le_search->text());
//        }
//        else if(ui->cb_search_firstName->isChecked())
//        {
//            filter = "firstName FULLLIKE ?||'%' "+QString(hasFilter?"AND ":"");
//            valList.append(ui->le_search->text());
//        }
//        else if(ui->cb_search_lastName->isChecked())
//        {
//            filter = "lastName LIKE ?||'%' "+QString(hasFilter?"AND ":"");
//            valList.append(ui->le_search->text());
//        }
//    }

    filter += m_filter;
    valList += m_filterValues;

//    qDebug() << "Member filter : " << m_filter;
//    qDebug() << "Filter : " << filter;

    if(!filter.isEmpty())
    {
        querStr += " WHERE " + filter;
    }

    //group by to avoid duplicate and sort by date displaying newest at the top
    querStr += QString{" GROUP BY %0.id ORDER BY %0.date DESC"}.arg(global::table_dives);

    if(enableDebug)
        qDebug() << "Dive search query : " << querStr;
    query.prepare(querStr);

    for(const auto& val : valList)
    {
        query.addBindValue(val);
    }

    query.exec();

    auto err = query.lastError();
    if(err.type() != QSqlError::ErrorType::NoError)
    {
        QString errStr{QString{"%0 : SQL error : %1"}.arg(__func__,err.text())};
        qCritical() << errStr;
    }


    qobject_cast<QSqlQueryModel*>(ui->tv_dives->model())->setQuery(std::move(query));

    if(db().isOpen() && !m_initGui)
    {
        //Setup gui
        for(int i{}; i < m_gui_divesColumnsNames.size();++i)
        {
            ui->tv_dives->model()->setHeaderData(i,Qt::Horizontal,m_gui_divesColumnsNames[i]);
        }
        //hide the last column
        ui->tv_dives->setColumnHidden(ui->tv_dives->model()->columnCount()-1,true);
        m_initGui = true;
    }
    ui->tv_dives->resizeColumnsToContents();
    return;
}

QVector<int> DiveSearch::getSelectedDivesId()
{
    auto indexes{ui->tv_dives->selectionModel()->selectedRows()};
    auto model{qobject_cast<QSqlQueryModel*>(ui->tv_dives->model())};

    QVector<int> out{};
    out.resize(indexes.size());

    for(int i{}; i < indexes.size(); ++i)
    {
        out[i] = model->data(indexes[i].siblingAtColumn(model->columnCount()-1)).value<int>();
    }
    return out;
}

QVector<int> DiveSearch::getDisplayedDivesId()
{
    auto model{qobject_cast<QSqlQueryModel*>(ui->tv_dives->model())};
    auto colIndex{model->columnCount()-1};

    QVector<int> out{};
    out.resize(model->rowCount());

    for(int i{}; i < model->rowCount();++i)
    {
        auto index{model->index(i,colIndex)};
        out[i] = model->data(index).toInt();
    }
    return out;
}

}//namespace gui

void gui::DiveSearch::on_tv_dives_doubleClicked(const QModelIndex &index)
{
    auto model{qobject_cast<QSqlQueryModel*>(ui->tv_dives->model())};
    //get the diver ID which should be the last column
    int diverId{model->data(index.siblingAtColumn(model->columnCount()-1)).value<int>()};

    emit divesSelected(QVector<int>{diverId});
}

