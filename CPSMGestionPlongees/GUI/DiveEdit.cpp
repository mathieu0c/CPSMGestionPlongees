#include "DiveEdit.h"
#include "ui_DiveEdit.h"

#include "../global.hpp"

#include "GUI/global.hpp"
#include "GUI/Dialog_EditFamily.h"

#include "GUI/DiverSearch.hpp"

#include "Info/Diver.h"

#include <QLineEdit>

namespace gui
{

DiveEdit::DiveEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiveEdit)
{
    ui->setupUi(this);

    auto lambdaConfigureDiverSearch{
        [&](DiverSearch* ds)
        {
            ds->setHiddenButton(true);
        }
    };

    ui->diverSearch_global->setSelectionColumns("lastName,firstName,level",{"Nom de famille","Prénom","Niveau"});
    ui->diverSearch_dive->setSqlJoins(" INNER JOIN %0 ON %1.id = %0.diverId",{global::table_divesMembers,global::table_divers});
    ui->diverSearch_dive->setSelectionColumns("lastName,firstName,level,diveType",{"Nom de famille","Prénom","Niveau","Type"});

    lambdaConfigureDiverSearch(ui->diverSearch_dive);
    lambdaConfigureDiverSearch(ui->diverSearch_global);

    refreshDiverSearchFilters_global();
    refreshDiverSearchFilters_dive();
//    ui->diverSearch_dive->setFilter("%0.id IN (SELECT %1.diverId FROM %1 WHERE %1.diveId = ?)",{global::table_divers,global::table_divesMembers},{});

    connect(ui->buttonBox,&QDialogButtonBox::rejected,this,&DiveEdit::on_buttonBox_rejected);
    connect(ui->buttonBox,&QDialogButtonBox::accepted,this,&DiveEdit::on_buttonBox_accepted);

    connect(ui->diverSearch_dive,&DiverSearch::refreshedDiverList,this,&DiveEdit::refreshDiversListComboBox);

//    connect(ui->le_lastName,&QLineEdit::textChanged,[&](const auto& in){
//            ui->le_lastName->setText(in.toUpper());
//            m_tempDiver.lastName = ui->le_lastName->text();});

}

DiveEdit::~DiveEdit()
{
    delete ui;
}


void DiveEdit::refreshSiteList(const
                               QStringList& list)
{
    ui->cb_diveSite->clear();
    for(const auto& e : list)
    {
        ui->cb_diveSite->addItem(e);
    }
}

void DiveEdit::refreshDiverSearchFilters_global()
{
    QString idList{};
    for(const auto& e : m_tempDive.divers)
    {
        idList += QString::number(e.id)+',';
    }
    idList.chop(1);//remove last ','
    ui->diverSearch_global->setFilter("%0.id NOT IN (%1)",{global::table_divers,idList},{});//global::table_divesMembers
}

void DiveEdit::refreshDiverSearchFilters_dive()
{
    //refresh filter values to allow only divers with id corresponding to m_temp.divers (QList<id>)
    QString idList{};
    for(const auto& e : m_tempDive.divers)
    {
        idList += QString::number(e.id)+',';
    }
    idList.chop(1);//remove last ','
    ui->diverSearch_dive->setFilter("%0.id IN (%1)",{global::table_divers,idList},{});//global::table_divesMembers
}

void DiveEdit::refreshDiversList()
{
    refreshDiverSearchFilters_global();
    refreshDiverSearchFilters_dive();
    ui->diverSearch_dive->refreshDiverList();
    ui->diverSearch_global->refreshDiverList();

//    auto model{ui->diverSearch_dive};
    refreshDiversListComboBox();
}

void DiveEdit::refreshDiversListComboBox()
{
    auto rowCount{ui->diverSearch_dive->getRowCount()};
    auto columnCount{ui->diverSearch_dive->getColumnCount()-1};//one column is hidden by default
    auto table{ui->diverSearch_dive->table()};

    for(int i{}; i < rowCount;++i)
    {
        auto index{ui->diverSearch_dive->indexAt(columnCount-1,i)};//modify pre-last column
            //the last one is the diver id
//        qDebug() << __func__ << "  " << index;
//        qDebug() << index.data();
        auto tempCb{new QComboBox()};
        auto diverId{ui->diverSearch_dive->getDiverIdAt(index)};
        tempCb->addItem(to_string(info::DiveType::exploration),diverId);//set the diver id as meta data
        tempCb->addItem(to_string(info::DiveType::technical),diverId);
        tempCb->setCurrentIndex(0);
        auto diveType{info::getDiveTypeForDiver(m_tempDive,diverId)};
        tempCb->setCurrentIndex((diveType == info::DiveType::exploration)?0:1);

        table->setIndexWidget(index,tempCb);//ownership of tempCb is given to table
        connect(qobject_cast<QComboBox*>(table->indexWidget(index)),&QComboBox::currentIndexChanged,
            this,&DiveEdit::slot_diveComboBox);
    }
}

void DiveEdit::slot_diveComboBox(int index)
{
    qDebug() << "''''''''' --- " << __func__ << " --- '''''''''";
    qDebug() << "##############################################";
    qDebug() << m_tempDive;
    qDebug() << "##############################################";
    QComboBox* box{qobject_cast<QComboBox*>(QObject::sender())};
    auto diverId{box->itemData(index).value<int>()};
    setDiveTypeForDiver(m_tempDive,diverId,info::diveTypefrom_string(box->currentText()));
    qDebug() << "##############################################";
    qDebug() << m_tempDive;
    qDebug() << "##############################################";
}

void DiveEdit::setDive(info::Dive diver){
    m_tempDive = std::move(diver);

    if(enableDebug)
    {
        qDebug() << "Setup diver in edition mode : ";
        qDebug() << m_tempDive;
    }

    ui->de_diveDate->setDate(m_tempDive.date);

//    ui->diverSearch_global->setFilterValues({m_tempDive.id});//refresh filter value (dive ID) for divers search
//    ui->diverSearch_dive->setFilterValues({m_tempDive.id});
//    ui->le_address->setText(m_tempDiver.address);
}

void DiveEdit::resetDive(){
    m_tempDive = info::Dive{};
    setDive(std::move(m_tempDive));
}


void DiveEdit::on_pb_diverToDive_clicked()
{
    auto diversIds{ui->diverSearch_global->getSelectedDiversId()};
    if(!diversIds.size())
        return;

    qDebug() << __func__ <<" ---------------------------------- ";
//    qDebug() << ui->diverSearch_dive->getDisplayedDiversId();

    for(const auto& e : diversIds)
    {
        qDebug() << "       Selected : " << e;
        m_tempDive.divers.append({e,info::DiveType::exploration});
    }

    refreshDiversList();
}


void DiveEdit::on_pb_diveToDiver_clicked()
{
    auto diversIds{ui->diverSearch_dive->getSelectedDiversId()};
    if(!diversIds.size())
        return;

    qDebug() << __func__ <<" ---------------------------------- ";
    qDebug() << m_tempDive;

    for(const auto& e : diversIds)
    {
        qDebug() << "       Selected : " << e;
//        m_tempDive.divers.append({e,info::DiveType::exploration});
    }

    info::removeDiversFromDive(m_tempDive,diversIds);
    qDebug() << m_tempDive;

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
