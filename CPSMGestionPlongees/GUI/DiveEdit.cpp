#include "DiveEdit.h"
#include "ui_DiveEdit.h"

#include "../global.hpp"

#include "GUI/global.hpp"
#include "GUI/Dialog_EditFamily.h"

namespace gui
{

DiveEdit::DiveEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiveEdit)
{
    ui->setupUi(this);

//    connect(ui->le_lastName,&QLineEdit::textChanged,[&](const auto& in){
//            ui->le_lastName->setText(in.toUpper());
//            m_tempDiver.lastName = ui->le_lastName->text();});

}

DiveEdit::~DiveEdit()
{
    delete ui;
}


void DiveEdit::setDive(info::Dive diver){
    m_tempDive = std::move(diver);

    if(enableDebug)
    {
        qDebug() << "Setup diver in edition mode : ";
        qDebug() << m_tempDive;
    }




//    ui->le_address->setText(m_tempDiver.address);
}

void DiveEdit::resetDive(){
    m_tempDive = info::Dive{};
    setDive(std::move(m_tempDive));
}

}//namespace gui

void gui::DiveEdit::on_buttonBox_accepted()
{
    //qDebug() << m_tempDiver;
    emit endEditing(m_tempDive);
}

void gui::DiveEdit::on_buttonBox_rejected()
{
    emit rejectedEditing();
}

void gui::DiveEdit::on_cb_gear_global_stateChanged(int arg1)
{
//    ui->cb_regulator->setChecked(arg1);
//    ui->cb_suit->setChecked(arg1);
//    ui->cb_computer->setChecked(arg1);
//    ui->cb_jacket->setChecked(arg1);
}
