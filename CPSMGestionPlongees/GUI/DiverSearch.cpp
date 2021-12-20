#include "DiverSearch.hpp"
#include "ui_DiverSearch.h"

#include "GUI/constSettings.hpp"
#include "GUI/global.hpp"

#include "../global.hpp"

#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>

#include <QModelIndexList>
#include <QModelIndex>

namespace gui
{

DiverSearch::DiverSearch(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiverSearch)
{
    ui->setupUi(this);

    setSelectionColumns("lastName,firstName,member,level,paidDives-diveCount",{"Nom de famille","Prénom","Membre","Niveau","Solde"});

    refreshDiverList();

    //refreshing diver list
    connect(ui->le_search,&QLineEdit::textChanged,[&](){refreshDiverList();});
    connect(ui->cb_search_firstName,&QCheckBox::stateChanged,[&](){refreshDiverList();});
    connect(ui->cb_search_lastName,&QCheckBox::stateChanged,[&](){refreshDiverList();});
}

DiverSearch::~DiverSearch()
{
    delete ui;
}

void DiverSearch::setHiddenButton(bool hide)
{
    ui->buttonBox->setVisible(!hide);
}

void DiverSearch::setSelectionMode(QAbstractItemView::SelectionMode mode)
{
    ui->tv_divers->setSelectionMode(mode);
}

void DiverSearch::setFilter(QString newFilter,const QStringList& argList,QVector<QVariant> filterValues)
{
    m_filter = std::move(newFilter);
    m_filterValues = std::move(filterValues);

    for(const auto& e : argList)//match argument list
    {
        m_filter = m_filter.arg(e);
    }
}

int DiverSearch::getRowCount(){
    return ui->tv_divers->model()->rowCount();
}

void DiverSearch::refreshDiverList()
{
    if(ui->tv_divers->model() == nullptr) //if there is no model set in the view
    {
        ui->tv_divers->setModel(new QSqlQueryModel(this));//create it
    }

    if(!db().open())//if the db isn't open : quit function
        return;

    emit askRefreshDiverList();

    QString querStr{"SELECT %0 FROM %1 INNER JOIN %2 ON %1.diverLevelId = %2.id INNER JOIN %3 ON %1.memberAddressId = %3.id"};
    querStr = querStr.arg(m_sql_diversColumns,global::table_divers,global::table_diverLevel,global::table_diversAddresses);

    //auto querStrFirstName{querStr+" WHERE firstName LIKE ?||'%'"};
    //auto querStrLastName{querStr+" WHERE lastName LIKE ?||'%'"};
    QSqlQuery query{db()};

    bool hasFilter{!m_filter.isEmpty()};
    QString filter{};
    QVector<QVariant> valList{};


    //if nothing is searched or there no search option selected
    if(ui->le_search->text().isEmpty() || (!ui->cb_search_firstName->isChecked() && !ui->cb_search_lastName->isChecked()))
    {
        //no filter
        //querStr += " ORDER BY lastName";
    }
    else
    {
        //querStr += " WHERE ";
        if(ui->cb_search_firstName->isChecked() && ui->cb_search_lastName->isChecked())
        {
            filter = "firstName LIKE ?||'%' OR lastName LIKE ?||'%' "+QString(hasFilter?"AND ":"");
            valList.append(ui->le_search->text());
            valList.append(ui->le_search->text());
        }
        else if(ui->cb_search_firstName->isChecked())
        {
            filter = "firstName LIKE ?||'%' "+QString(hasFilter?"AND ":"");
            valList.append(ui->le_search->text());
        }
        else if(ui->cb_search_lastName->isChecked())
        {
            filter = "lastName LIKE ?||'%' "+QString(hasFilter?"AND ":"");
            valList.append(ui->le_search->text());
        }

//        //select people where firstname begin with ...
//        if(ui->cb_search_firstName->isChecked())
//        {
//            querStr += "firstName LIKE ?||'%'";
//            if(ui->cb_search_lastName->isChecked())
//                querStr += " OR ";
//        }
//        //select people where lastname begin with ...
//        if(ui->cb_search_lastName->isChecked())
//        {
//            querStr += "lastName LIKE ?||'%'";
//        }
    }

    filter += m_filter;
    valList += m_filterValues;

    qDebug() << "Member filter : " << m_filter;
    qDebug() << "Filter : " << filter;

    if(!filter.isEmpty())
    {
        querStr += " WHERE " + filter;
    }

    querStr += " ORDER BY lastName";

    if(enableDebug)
        qDebug() << "Diver search query : " << querStr;
    query.prepare(querStr);

//    if(!ui->le_search->text().isEmpty())
//    {
//        if(ui->cb_search_firstName->isChecked())
//            query.addBindValue(ui->le_search->text());
//        if(ui->cb_search_lastName->isChecked())
//            query.addBindValue(ui->le_search->text());
//    }
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


    qobject_cast<QSqlQueryModel*>(ui->tv_divers->model())->setQuery(query);

    if(db().isOpen() && !m_initGui)
    {
        //Setup gui
        for(int i{}; i < m_gui_diversColumnsNames.size();++i)
        {
            ui->tv_divers->model()->setHeaderData(i,Qt::Horizontal,m_gui_diversColumnsNames[i]);
        }
        //hide the last column
        ui->tv_divers->setColumnHidden(ui->tv_divers->model()->columnCount()-1,true);
        m_initGui = true;
    }
    else
    {
        return;
    }
}

QVector<int> DiverSearch::getSelectedDiversId()
{
    auto indexes{ui->tv_divers->selectionModel()->selectedRows()};
    auto model{qobject_cast<QSqlQueryModel*>(ui->tv_divers->model())};

    QVector<int> out{};
    out.resize(indexes.size());

    for(int i{}; i < indexes.size(); ++i)
    {
        out[i] = model->data(indexes[i].siblingAtColumn(model->columnCount()-1)).value<int>();
    }
    return out;
}

QVector<int> DiverSearch::getDisplayedDiversId()
{
    auto model{qobject_cast<QSqlQueryModel*>(ui->tv_divers->model())};
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

//void gui::MainWindow::on_tv_divers_doubleClicked(const QModelIndex &index)
//{
//    auto id{index.row()+1};
//    auto tempDiver{info::readDiverFromDB(id,QSqlDatabase::database(),global::table_divers)};

//    qDebug() << " ------ Read DIVER :";
//    qDebug() << tempDiver;

//    ui->pg_editDiver->setDiver(std::move(tempDiver));
//    ui->tab_divers->setCurrentIndex(1);
//}

}//namespace gui

void gui::DiverSearch::on_tv_divers_doubleClicked(const QModelIndex &index)
{
    auto model{qobject_cast<QSqlQueryModel*>(ui->tv_divers->model())};
    //get the diver ID which should be the last column
    int diverId{model->data(index.siblingAtColumn(model->columnCount()-1)).value<int>()};

    emit diversSelected(QVector<int>{diverId});
}
