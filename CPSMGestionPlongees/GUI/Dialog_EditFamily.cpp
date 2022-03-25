#include "Dialog_EditFamily.h"
#include "ui_Dialog_EditFamily.h"

#include "../global.hpp"

#include "DBApi/Database.hpp"

#include <QSqlDatabase>
#include <QWidget>

#include <QDebug>

namespace gui
{

Dialog_EditFamily::Dialog_EditFamily(QString dbName,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_EditFamily)
{
    ui->setupUi(this);

    setDBName(dbName);

    if(parent)
        this->setModal(true);

    ui->diverSearch->setHiddenButton(true);
    ui->diverSearch->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->diverSearch->setSelectionColumns(QString{"lastName,firstName,%0.address,%0.postalCode,%0.city"}.arg(global::table_diversAddresses),{"Nom de famille","Prénom","Addresse","CP","Ville"});
    ui->diverSearch->refreshDiverList();
}

Dialog_EditFamily::~Dialog_EditFamily()
{
    delete ui;
}

std::optional<int> Dialog_EditFamily::selectedAddressId()
{
    auto db{this->db()};

    auto diverIdList{ui->diverSearch->getSelectedDiversId()};

    if(diverIdList.size() == 0)
        return {};

    auto diverId{diverIdList[0]};
    auto addressQueryResult{db::querySelect(db,"SELECT memberAddressId FROM %0 WHERE id=?",{global::table_divers},{diverId})};
    if(addressQueryResult.size() == 0)
        return {};

    return {addressQueryResult[0][0].toInt()};
}


void Dialog_EditFamily::setDBName(QString name)
{
    m_dbName = std::move(name);
    ui->diverSearch->setDBName(m_dbName);
}

void Dialog_EditFamily::refreshDiverList()
{
    ui->diverSearch->refreshDiverList();
}

void Dialog_EditFamily::setAddressId(int id)
{
    m_addressId = id;

    auto db{this->db()};

    auto diverIdsSharingAddress{db::readLFromDB<int>(db,[&](const QSqlQuery& query){
            return query.value(0).value<int>();
        },"SELECT %0.id FROM %0 WHERE memberAddressId=?",{global::table_divers},{id})};

    if(diverIdsSharingAddress.size() < 1 || (
        diverIdsSharingAddress.size() == 1 && diverIdsSharingAddress.contains(m_diverId)
    ))
    {
        setMode(EditMode::selectFamily);
    }
    else
    {
        setMode(EditMode::quitFamily);
    }
}

void Dialog_EditFamily::setMode(EditMode mode)
{
    m_currentMode = mode;
    refreshSearchFilter(mode);

    if(mode == EditMode::selectFamily)
    {
        ui->lbl_text->setText(tr("Sélectionnez l'un des membre d'une famille pour partager l'adresse :"));
        ui->buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Help);
    }
    else if(mode == EditMode::quitFamily)
    {
        ui->lbl_text->setText(tr("Souhaitez-vous supprimer ce plongeur de la famille suivante ?"));
        ui->buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Yes | QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Help);
    }
}

std::optional<int> Dialog_EditFamily::getAddressId(int diverId,int addressId, QWidget* parent, QString dbName)
{
    Dialog_EditFamily dial{dbName,parent};

    dial.setDBName(dbName);
    dial.setDiverId(diverId);
    dial.setAddressId(addressId);

    auto ans{dial.exec()};

    if(ans==QDialog::Accepted)
    {
        if(dial.mode() == EditMode::selectFamily)
        {
            auto tempId{dial.selectedAddressId()};
            if(tempId)
            {
                return {tempId.value()};
            }
            //else
            return {};
        }
        else if(dial.mode() == EditMode::quitFamily)
        {
            return {-1};
        }
    }
    //else

    return {};
}

//--------------- SLOTS

void Dialog_EditFamily::refreshSearchFilter(EditMode mode)
{
    if(mode == EditMode::selectFamily)
    {
        ui->diverSearch->setFilter(QString{"%1.id != ?"},{global::table_divers},{m_diverId});
    }
    else if(mode == EditMode::quitFamily)
    {
        ui->diverSearch->setFilter(QString{"%0.id = ? AND %1.id != ?"},{global::table_diversAddresses,global::table_divers},{m_addressId,m_diverId});
    }
    ui->diverSearch->refreshDiverList();
}

}//namespace gui
