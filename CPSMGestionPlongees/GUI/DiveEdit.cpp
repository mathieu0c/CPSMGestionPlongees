#include "DiveEdit.h"
#include "ui_DiveEdit.h"

#include "../global.hpp"

#include "GUI/global.hpp"
#include "GUI/Dialog_EditFamily.h"

#include "GUI/DiverSearch.hpp"

#include "DBApi/Database.hpp"
#include "DBApi/DataStructs.hpp"
#include "DBApi/DBDive.hpp"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

namespace gui
{

DiveEdit::DiveEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiveEdit)
{
    ui->setupUi(this);

    auto lambdaConfigureDiverSearch{
        [&](auto* ds)
        {
            ds->setHiddenButton(true);
        }
    };

    ui->diverSearch_global->setSelectionColumns("lastName,firstName,level",{"Nom de famille","Prénom","Niv."});
    //note that we select two times level to create a column that will be used for DiveType's combobox
    ui->diverSearch_dive->setSelectionColumns({"Nom de famille","Prénom","Niv.","Type"});

    lambdaConfigureDiverSearch(ui->diverSearch_dive);
    lambdaConfigureDiverSearch(ui->diverSearch_global);

    refreshDiverSearchFilters_global();
    refreshDiverSearchFilters_dive();
//    ui->diverSearch_dive->setFilter("%0.id IN (SELECT %1.diverId FROM %1 WHERE %1.diveId = ?)",{global::table_divers,global::table_divesMembers},{});

    connect(ui->buttonBox,&QDialogButtonBox::rejected,this,&DiveEdit::on_buttonBox_rejected);
    connect(ui->buttonBox,&QDialogButtonBox::accepted,this,&DiveEdit::on_buttonBox_accepted);

//    connect(ui->diverSearch_dive,&DiverSearch::refreshedDiverList,this,&DiveEdit::refreshDiversListComboBox);

    connect(ui->de_diveDate,&QDateEdit::dateChanged,[&](const auto& in){
            m_tempDive.date = in;});
    connect(ui->te_diveTime,&QTimeEdit::timeChanged,[&](const auto& in){
            m_tempDive.time = in;});
    connect(ui->cb_diveSite,&QComboBox::currentIndexChanged,[&](const auto& in){
            m_tempDive.diveSiteId = ui->cb_diveSite->currentData().toInt();});

}

DiveEdit::~DiveEdit()
{
    delete ui;
}


void DiveEdit::refreshSiteList(const QString& siteTable)
{
    ui->cb_diveSite->clear();

    auto divingSites{db::querySelect(QSqlDatabase::database(),"SELECT id,name FROM %0 ORDER BY name",{siteTable},{})};

    for(const auto& line : divingSites)
    {
        ui->cb_diveSite->addItem(line[1].toString(),line[0]);
    }

    ui->cb_diveSite->setCurrentIndex(0);
}

void DiveEdit::setEditable(bool enable)
{
    m_isEditable = enable;
    global::tools::applyToChildren<QComboBox*>(this,[&](QComboBox* box){box->setEnabled(enable);});
    global::tools::applyToChildren<QDateEdit*>(this,[&](QDateEdit* box){box->setReadOnly(!enable);});
    global::tools::applyToChildren<QTimeEdit*>(this,[&](QTimeEdit* box){box->setReadOnly(!enable);});

    ui->diverSearch_dive->setEditable(enable);

    ui->pb_diveToDiver->setEnabled(enable);
    ui->pb_diverToDive->setEnabled(enable);

    //disable combo box for type selection
//    auto rowCount{ui->diverSearch_dive->getRowCount()};
//    auto columnCount{ui->diverSearch_dive->getColumnCount()-1};//one column is hidden by default
//    auto table{ui->diverSearch_dive->table()};

//    for(int i{}; i < rowCount;++i)
//    {
//        auto index{ui->diverSearch_dive->indexAt(columnCount-1,i)};//modify pre-last column (comboboxes for type)
//            //the last one is the diver id

//        auto cb{qobject_cast<QComboBox*>(table->indexWidget(index))};
//        if(cb != nullptr)
//            cb->setEnabled(enable);
//    }

//    //hide/show (un)necessary widgets
//    //hide/show buttons
//    global::tools::applyToChildren<QPushButton*>(this,[&](QPushButton* pb){pb->setVisible(enable);});
//    ui->diverSearch_global->setVisible(enable);
}

void DiveEdit::refreshDiverSearchFilters_global()
{
    QString idList{ui->diverSearch_dive->formattedDiversIds()};

//    for(const auto& e : m_tempDive.divers)
//    {
//        idList += QString::number(e.id)+',';
//    }
//    idList.chop(1);//remove last ','
    ui->diverSearch_global->setFilter("%0.id NOT IN %1",{global::table_divers,idList},{});//global::table_divesMembers
}

void DiveEdit::refreshDiverSearchFilters_dive()
{
    //refresh filter values to allow only divers with id corresponding to m_temp.divers (QList<id>)
    QString idList{};
//    for(const auto& e : m_tempDive.divers)
//    {
//        idList += QString::number(e.id)+',';
//    }
//    idList.chop(1);//remove last ','
//    ui->diverSearch_dive->setFilter("%0.id IN (%1)",{global::table_divers,idList},{});//global::table_divesMembers
}

void DiveEdit::refreshDiversList()
{
    refreshDiverSearchFilters_global();
    refreshDiverSearchFilters_dive();
    ui->diverSearch_dive->refreshDiverList(QSqlDatabase::database(),global::table_diverLevel);
    ui->diverSearch_global->refreshDiverList();

//    auto model{ui->diverSearch_dive};
    refreshDiversListComboBox();
}

void DiveEdit::refreshDiversListComboBox()
{
    using data::to_string;
//    return;
//    auto rowCount{ui->diverSearch_dive->getRowCount()};
//    auto columnCount{ui->diverSearch_dive->getColumnCount()-1};//one column is hidden by default
//    auto table{ui->diverSearch_dive->table()};

//    for(int i{}; i < rowCount;++i)
//    {
//        auto index{ui->diverSearch_dive->indexAt(columnCount-1,i)};//modify pre-last columndiveMemberQueryOffset
//            //the last one is the diver id
////        qDebug() << __func__ << "  " << index;
////        qDebug() << index.data();
//        auto tempCb{new QComboBox()};
//        auto diverId{ui->diverSearch_dive->getDiverIdAt(index)};
//        tempCb->addItem(to_string(data::DiveType::exploration),diverId);//set the diver id as meta data
//        tempCb->addItem(to_string(data::DiveType::technical),diverId);
//        tempCb->setCurrentIndex(0);
//        auto diveType{data::getDiveTypeForDiver(m_tempDive,diverId)};
//        tempCb->setCurrentIndex((diveType == data::DiveType::exploration)?0:1);
//        if(!m_isEditable)//if the widget isn't in editable mode
//            tempCb->setEnabled(false);
//        //it's enabled by default

//        auto oldCb{qobject_cast<QComboBox*>(table->indexWidget(index))};

//        table->setIndexWidget(index,tempCb);//ownership of tempCb is given to table
//        connect(qobject_cast<QComboBox*>(table->indexWidget(index)),&QComboBox::currentIndexChanged,
//            this,&DiveEdit::slot_diveComboBox);
//        delete oldCb;//free no more used objects
//    }
}

void DiveEdit::slot_diveComboBox(int index)
{
    QComboBox* box{qobject_cast<QComboBox*>(QObject::sender())};
    auto diverId{box->itemData(index).value<int>()};
    data::setDiveTypeForDiver(m_tempDive,diverId,data::diveTypefrom_string(box->currentText()));
}

void DiveEdit::setDive(data::Dive dive){
    m_tempDive = std::move(dive);
    ui->diverSearch_dive->setDivers(m_tempDive.diver);

    using db::operator<<;

    if(enableDebug)
    {
        qDebug() << "Setup diver in edition mode : ";
        qDebug() << m_tempDive;
    }

    ui->de_diveDate->setDate(m_tempDive.date);
    ui->te_diveTime->setTime(m_tempDive.time);

    auto cbIndex{ui->cb_diveSite->findData(QVariant(m_tempDive.diveSiteId))};
    if(cbIndex != -1)
    {
        ui->cb_diveSite->setCurrentIndex(cbIndex);
    }
    else
        ui->cb_diveSite->setCurrentIndex(0);

    refreshDiversList();
}

void DiveEdit::resetDive(){
    m_tempDive = data::Dive{};
    m_tempDive.date = QDate::currentDate();
    m_tempDive.time = QTime::currentTime();
    setDive(std::move(m_tempDive));
}

/*
 * Function called when a list of diver is required to move from the global
 * listing to the dive listing
 * dive -> diveMembers
 * diver -> All available divers
*/
void DiveEdit::on_pb_diverToDive_clicked()
{
    auto diversIds{ui->diverSearch_global->getSelectedDiversId()};
    if(!diversIds.size())
        return;

//    qDebug() << __func__ <<" ---------------------------------- ";
//    qDebug() << ui->diverSearch_dive->getDisplayedDiversId();
//    qDebug() << m_tempDive;

    //assuming divers displayed are not already in the dive's divers list
    m_tempDive.diver += db::readDiveMembersFromDB(m_tempDive.id,diversIds,QSqlDatabase::database(),global::table_divers);


//    qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~";
//    qDebug() << m_tempDive;

    refreshDiversList();
}


//dive -> diveMembers
//diver -> All available divers
void DiveEdit::on_pb_diveToDiver_clicked()
{
    auto diversIds{ui->diverSearch_dive->selectedDiversId()};
//    if(!diversIds.size())
//        return;

//    qDebug() << __func__ <<" ---------------------------------- ";
//    qDebug() << m_tempDive;

    data::removeDiversFromDive(m_tempDive,diversIds);
//    qDebug() << m_tempDive;

    refreshDiversList();
}

}//namespace gui

void gui::DiveEdit::on_buttonBox_accepted()
{
//    qDebug() << m_tempDive;
    emit endEditing(m_tempDive);
}

void gui::DiveEdit::on_buttonBox_rejected()
{
//    qDebug() << "REJECTED";
    emit rejectedEditing();
}
