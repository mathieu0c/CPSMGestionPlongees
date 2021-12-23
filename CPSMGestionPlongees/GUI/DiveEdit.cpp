#include "DiveEdit.h"
#include "ui_DiveEdit.h"

#include "../global.hpp"

#include "GUI/global.hpp"
#include "GUI/Dialog_EditFamily.h"

#include "GUI/DiverSearch.hpp"

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
            ds->setSelectionColumns("lastName,firstName,level",{"Nom de famille","Prénom","Niveau"});
            ds->refreshDiverList();
        }
    };

    lambdaConfigureDiverSearch(ui->diverSearch_dive);
    lambdaConfigureDiverSearch(ui->diverSearch_global);

    connect(ui->buttonBox,&QDialogButtonBox::rejected,this,&DiveEdit::on_buttonBox_rejected);
    connect(ui->buttonBox,&QDialogButtonBox::accepted,this,&DiveEdit::on_buttonBox_accepted);

//    connect(ui->le_lastName,&QLineEdit::textChanged,[&](const auto& in){
//            ui->le_lastName->setText(in.toUpper());
//            m_tempDiver.lastName = ui->le_lastName->text();});

}

DiveEdit::~DiveEdit()
{
    delete ui;
}


void DiveEdit::refreshSiteList(const QStringList& list)
{
    ui->cb_diveSite->clear();
    for(const auto& e : list)
    {
        ui->cb_diveSite->addItem(e);
    }
}

void DiveEdit::refreshDiversList()
{
    ui->diverSearch_dive->refreshDiverList();
    ui->diverSearch_global->refreshDiverList();
}

void DiveEdit::setDive(info::Dive diver){
    m_tempDive = std::move(diver);

    if(enableDebug)
    {
        qDebug() << "Setup diver in edition mode : ";
        qDebug() << m_tempDive;
    }

    ui->de_diveDate->setDate(m_tempDive.date);

//    ui->le_address->setText(m_tempDiver.address);
}

void DiveEdit::resetDive(){
    m_tempDive = info::Dive{};
    setDive(std::move(m_tempDive));
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
