#include "DiverEdit.h"
#include "ui_DiverEdit.h"

#include "../global.hpp"

#include "GUI/global.hpp"
#include "GUI/Dialog_EditFamily.h"

namespace gui
{

DiverEdit::DiverEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiverEdit)
{
    ui->setupUi(this);

    connect(ui->le_lastName,&QLineEdit::textChanged,[&](const auto& in){
            ui->le_lastName->setText(in.toUpper());
            m_tempDiver.lastName = ui->le_lastName->text();});
    connect(ui->le_firstname,&QLineEdit::textChanged,[&](const auto& in){
            m_tempDiver.firstName = in;});
    connect(ui->de_birthDate,&QDateEdit::dateChanged,[&](const auto& in){
            m_tempDiver.birthDate = in;});
    connect(ui->le_mail,&QLineEdit::textChanged,[&](const auto& in){
            m_tempDiver.email = in;
            ui->le_mail->setToolTip(in);});

    connect(ui->le_address,&QLineEdit::textChanged,[&](const auto& in){
            m_tempDiver.address.address = in;
            ui->le_address->setToolTip(in);});
    connect(ui->le_postalCode,&QLineEdit::textChanged,[&](const auto& in){
            m_tempDiver.address.postalCode = in;});
    connect(ui->le_city,&QLineEdit::textChanged,[&](const auto& in){
            m_tempDiver.address.city = in;});

    connect(ui->le_license,&QLineEdit::textChanged,[&](const auto& in){
            m_tempDiver.licenseNumber = in;
            ui->le_license->setToolTip(in);});
    connect(ui->cb_level,&QComboBox::currentIndexChanged,[&](const auto& in){
            m_tempDiver.diverLevelId = in+1;});//diver level id begins at 1
    connect(ui->cb_member,&QCheckBox::stateChanged,[&](const auto& in){
            m_tempDiver.member = bool(in);});
    connect(ui->le_mail,&QLineEdit::textChanged,[&](const auto& in){
            m_tempDiver.email = in;});
    connect(ui->le_phone,&QLineEdit::textChanged,[&](const auto& in){
            m_tempDiver.phoneNumber = in;});

    connect(ui->sb_payment,&QSpinBox::valueChanged,[&](const auto&){
            computeDivingCount();
            /*ui->sb_sold->setValue(m_tempDiver.sold());*/});

    connect(ui->cb_regulator,&QCheckBox::stateChanged,[&](const auto& in){
            m_tempDiver.gear_regulator = bool(in);});
    connect(ui->cb_suit,&QCheckBox::stateChanged,[&](const auto& in){
            m_tempDiver.gear_suit = bool(in);});
    connect(ui->cb_computer,&QCheckBox::stateChanged,[&](const auto& in){
            m_tempDiver.gear_computer = bool(in);});
    connect(ui->cb_jacket,&QCheckBox::stateChanged,[&](const auto& in){
            m_tempDiver.gear_jacket = bool(in);});

    m_tempDiver.birthDate = ui->de_birthDate->date();
    ui->de_certificate->setDate(QDate::currentDate());

    connect(ui->pb_paymentPlus,&QPushButton::clicked,[&](){
            ui->sb_payment->setValue(ui->sb_payment->value()+1);});
    connect(ui->pb_paymentMinus,&QPushButton::clicked,[&](){
            ui->sb_payment->setValue(ui->sb_payment->value()-1);});

    ui->diveSearch->setHiddenButton(true);
    ui->diveSearch->setSelectionColumns(QString{"date,%0.name"}.arg(
                            global::table_divingSites,
                            global::table_divesMembers
                            ),
                        {"Date","Site","Nombre de plongeurs"});
}

DiverEdit::~DiverEdit()
{
    delete ui;
}

void DiverEdit::refreshLevelList(const QStringList& list)
{
    ui->cb_level->clear();
    for(const auto& e : list)
    {
        ui->cb_level->addItem(e);
    }
}

void DiverEdit::setAddress(info::Address address)
{
    m_tempDiver.address = std::move(address);

    ui->le_address->setText(m_tempDiver.address.address);
    ui->le_postalCode->setText(m_tempDiver.address.postalCode);
    ui->le_city->setText(m_tempDiver.address.city);
}

void DiverEdit::setDiver(info::Diver diver){
    m_tempDiver = std::move(diver);

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
    ui->cb_level->setCurrentIndex(m_tempDiver.diverLevelId-1);
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
    m_tempDiver = info::Diver{};
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
        m_tempDiver.address = info::readAddressFromDB(id,QSqlDatabase::database(),global::table_diversAddresses);
    else
        m_tempDiver.address = {};

    setDiver(m_tempDiver);
}

}//namespace gui

void gui::DiverEdit::on_buttonBox_accepted()
{
    //qDebug() << m_tempDiver;
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
