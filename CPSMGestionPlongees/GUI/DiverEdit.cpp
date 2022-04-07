#include "DiverEdit.h"
#include "ui_DiverEdit.h"

#include "../global.hpp"

#include "GUI/global.hpp"
#include "GUI/Dialog_EditFamily.h"
#include "GUI/DiveEdit.h"
#include <QMessageBox>

#include "DBApi/DataStructs.hpp"
#include "DBApi/DBDive.hpp"
#include "DBApi/DBAddress.hpp"
#include "DBApi/DBApi.hpp"

#include "DBApi/DBAddress.hpp"

namespace gui
{

DiverEdit::DiverEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiverEdit)
{
    ui->setupUi(this);

    connect(ui->le_lastName,&QLineEdit::textChanged,this,[&](const auto& in){
            ui->le_lastName->setText(in.toUpper());
            m_tempDiver.lastName = ui->le_lastName->text();});
    connect(ui->le_firstname,&QLineEdit::textChanged,this,[&](const auto& in){
            m_tempDiver.firstName = in;});
    connect(ui->de_birthDate,&QDateEdit::dateChanged,this,[&](const auto& in){
            m_tempDiver.birthDate = in;});
    connect(ui->le_mail,&QLineEdit::textChanged,this,[&](const auto& in){
            m_tempDiver.email = in;
            ui->le_mail->setToolTip(in);});

    connect(ui->le_address,&QLineEdit::textChanged,this,[&](const auto& in){
            m_tempDiver.address.address = in;
            ui->le_address->setToolTip(in);});
    connect(ui->le_postalCode,&QLineEdit::textChanged,this,[&](const auto& in){
            m_tempDiver.address.postalCode = in;});
    connect(ui->le_city,&QLineEdit::textChanged,this,[&](const auto& in){
            m_tempDiver.address.city = in;});

    connect(ui->le_license,&QLineEdit::textChanged,this,[&](const auto& in){
            m_tempDiver.licenseNumber = in;
            ui->le_license->setToolTip(in);});
    connect(ui->cb_level,&QComboBox::currentIndexChanged,this,[&](const auto&){
            auto newLevelId{ui->cb_level->currentData().toInt()};
            m_tempDiver.diverLevelId = newLevelId;
        });//diver level id begins at 1
    connect(ui->cb_member,&QCheckBox::stateChanged,this,[&](const auto& in){
            m_tempDiver.member = bool(in);});
    connect(ui->le_mail,&QLineEdit::textChanged,this,[&](const auto& in){
            m_tempDiver.email = in;});
    connect(ui->le_phone,&QLineEdit::textChanged,this,[&](const auto& in){
            m_tempDiver.phoneNumber = in;});

    connect(ui->sb_payment,&QSpinBox::valueChanged,this,[&](const auto&){
            computeDivingCount();
            /*ui->sb_sold->setValue(m_tempDiver.sold());*/});

    connect(ui->cb_regulator,&QCheckBox::stateChanged,this,[&](const auto& in){
            m_tempDiver.gear_regulator = bool(in);});
    connect(ui->cb_suit,&QCheckBox::stateChanged,this,[&](const auto& in){
            m_tempDiver.gear_suit = bool(in);});
    connect(ui->cb_computer,&QCheckBox::stateChanged,this,[&](const auto& in){
            m_tempDiver.gear_computer = bool(in);});
    connect(ui->cb_jacket,&QCheckBox::stateChanged,this,[&](const auto& in){
            m_tempDiver.gear_jacket = bool(in);});

    m_tempDiver.birthDate = ui->de_birthDate->date();
    ui->de_certificate->setDate(QDate::currentDate());

    connect(ui->pb_paymentPlus,&QPushButton::clicked,this,[&](){
            ui->sb_payment->setValue(ui->sb_payment->value()+1);});
    connect(ui->pb_paymentMinus,&QPushButton::clicked,this,[&](){
            ui->sb_payment->setValue(ui->sb_payment->value()-1);});

    ui->diveSearch->setHiddenButton(true);
    ui->diveSearch->setSelectionColumns(QString{"date,%0.name,diveType"}.arg(
                            global::table_divingSites
                            ),
                        {"Date","Site","Type"});

    connect(ui->diveSearch,&gui::DiveSearch::divesSelected,this,[&](QVector<int> idList){
        auto dive{db::readDiveFromDB(idList[0],QSqlDatabase::database(),global::table_dives,global::table_divingSites,
                                    global::table_divesMembers,global::table_divers)};
        gui::DiveEdit::displayDive(dive,this);
    });
}

DiverEdit::~DiverEdit()
{
    delete ui;
}

void DiverEdit::refreshLevelList(QVector<data::DiverLevel> levels,bool sortByAlphabetical)
{
    ui->cb_level->clear();
    m_diverLevelIdIndexMap.clear();

    std::function<bool(const data::DiverLevel&,const data::DiverLevel&)> sortFunction;

    if(sortByAlphabetical)
    {
        sortFunction = [&](const data::DiverLevel& e,const data::DiverLevel& r){
                return e.level.toLower() < r.level.toLower();
            };
    }
    else
    {
        sortFunction = [&](const data::DiverLevel& e,const data::DiverLevel& r){
                return e.sortValue < r.sortValue;
            };
    }

    std::sort(levels.begin(),levels.end(),sortFunction);
    int i{};
    for(const auto& e : levels)
    {
        ui->cb_level->addItem(e.level,e.id);
        m_diverLevelIdIndexMap[e.id] = i;
        ++i;
    }
}

void DiverEdit::setAddress(data::Address address)
{
    m_tempDiver.address = std::move(address);

    ui->le_address->setText(m_tempDiver.address.address);
    ui->le_postalCode->setText(m_tempDiver.address.postalCode);
    ui->le_city->setText(m_tempDiver.address.city);
}

void DiverEdit::setDiver(data::Diver diver){
    m_tempDiver = std::move(diver);

    using db::operator<<;

    if(enableDebug)
    {
        qDebug() << "Setup diver in edition mode : ";
        qDebug() << m_tempDiver;
    }

    ui->le_firstname->setText(m_tempDiver.firstName);
    ui->le_lastName->setText(m_tempDiver.lastName);
    ui->de_birthDate->setDate(m_tempDiver.birthDate);
    ui->le_license->setText(m_tempDiver.licenseNumber);
    ui->cb_member->setChecked(m_tempDiver.member);
    try
    {
        ui->cb_level->setCurrentIndex(m_diverLevelIdIndexMap[m_tempDiver.diverLevelId]);//index isn't corresponding to level
    } catch (const std::out_of_range& e)
    {
        ui->cb_level->setCurrentIndex(0);
    }
    ui->de_certificate->setDate(m_tempDiver.certifDate);

    setAddress(m_tempDiver.address);
    ui->le_mail->setText(m_tempDiver.email);
    ui->le_phone->setText(m_tempDiver.phoneNumber);

    ui->cb_regulator->setChecked(m_tempDiver.gear_regulator);
    ui->cb_suit->setChecked(m_tempDiver.gear_suit);
    ui->cb_computer->setChecked(m_tempDiver.gear_computer);
    ui->cb_jacket->setChecked(m_tempDiver.gear_jacket);

    m_tempDiverOriginalPaidDives = m_tempDiver.paidDives;
    ui->sb_payment->setValue(0);


    computeDivingCount();

    ui->diveSearch->setFilter("%0.diverId = ?",{global::table_divesMembers},{diver.id});
    ui->diveSearch->refreshDivesList();

//    ui->le_address->setText(m_tempDiver.address);
}

void DiverEdit::computeDivingCount()
{
    m_tempDiver.paidDives = m_tempDiverOriginalPaidDives + ui->sb_payment->value();
    ui->sb_diveCount->setValue(m_tempDiver.diveCount);
    ui->sb_sold->setValue(m_tempDiver.sold());
}

void DiverEdit::resetDiver(){
    m_tempDiver = data::Diver{};
    setDiver(std::move(m_tempDiver));
}


void DiverEdit::on_pb_family_clicked()
{
//    qDebug() << "---\t\t" << __func__;
    auto tempId{Dialog_EditFamily::getAddressId(m_tempDiver.id,m_tempDiver.address.id,this)};
    if(!tempId)//if the family edition was cancelled
    {
//        qDebug() << "No new id";
        return;
    }

    auto id{tempId.value()};
//    qDebug() << "New id : " << id;
    if(id != -1)
        m_tempDiver.address = db::readAddressFromDB(id,QSqlDatabase::database(),global::table_diversAddresses);
    else
        m_tempDiver.address = {};

    setDiver(m_tempDiver);
}

}//namespace gui

void gui::DiverEdit::on_buttonBox_accepted()
{
    //qDebug() << m_tempDiver;
    auto diverIdsSharingAddress{db::readLFromDB<int>(QSqlDatabase::database(),[&](const QSqlQuery& query){
            return query.value(0).value<int>();
        },"SELECT %0.id FROM %0 WHERE memberAddressId=?",
        {global::table_divers},{m_tempDiver.address.id})};

    if(!diverIdsSharingAddress.contains(m_tempDiver.id))
        diverIdsSharingAddress.append(m_tempDiver.id);

    if(diverIdsSharingAddress.size() > 1)//if multiple divers shares this address
    {
        auto refAddr{db::readAddressFromDB(m_tempDiver.address.id,QSqlDatabase::database(),global::table_diversAddresses)};

        if(refAddr.id < 0)
            throw std::runtime_error{QString{"%0 : Abnormal address id got when read from the DB"}.arg(__CURRENT_PLACE__).toStdString()};
        if(m_tempDiver.address != refAddr)
        {
            auto ans{QMessageBox::question(this,"Confirmation",
                                           QString{"Vous êtes sur le point de modifier l'adresse de"
                                                   " %0 personnes\n"
                                                   "Souhaitez-vous continuer ?"}.arg(diverIdsSharingAddress.size()))};
            if(ans == QMessageBox::No)
            {
                QMessageBox::information(this,"Information","Si vous souhaitez utiliser une adresse différente "
                                                            "pour cette personne uniquement, pensez à la"
                                                            " retirer de la famille dans laquelle elle se trouve.");
                return;
            }
        }
    }

    emit endEditing(m_tempDiver);
}

void gui::DiverEdit::on_buttonBox_rejected()
{
    emit rejectedEditing();
}

void gui::DiverEdit::on_cb_gear_global_stateChanged(int arg1)
{
    ui->cb_regulator->setChecked(arg1);
    ui->cb_suit->setChecked(arg1);
    ui->cb_computer->setChecked(arg1);
    ui->cb_jacket->setChecked(arg1);
}
