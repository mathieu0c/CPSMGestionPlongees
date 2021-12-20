#include "Dialog_ConfirmDiverDeletion.h"
#include "ui_Dialog_ConfirmDiverDeletion.h"

#include "GUI/global.hpp"

namespace gui
{

Dialog_ConfirmDiverDeletion::Dialog_ConfirmDiverDeletion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_ConfirmDiverDeletion)
{
    ui->setupUi(this);

    if(parent)
        this->setModal(true);
}

Dialog_ConfirmDiverDeletion::~Dialog_ConfirmDiverDeletion()
{
    delete ui;
}

void Dialog_ConfirmDiverDeletion::setDiverList(const QStringList& diverList)
{
    for(const auto& e : diverList)
    {
        ui->lw_diverList->addItem(e);
    }
}

bool Dialog_ConfirmDiverDeletion::confirmDeletion(const QStringList& diverList,QWidget* parent)
{
    auto dial{Dialog_ConfirmDiverDeletion(parent)};
    dial.ui->lw_diverList->addItems(diverList);

    auto ans{dial.exec()};

    if(ans == QDialog::Accepted)
    {
        if(enableDebug)
        {
            qDebug() <<"Dialog_ConfirmDiverDeletion::" << __func__ <<" : Confirmed deletion of " << diverList;
        }

        return true;
    }//End if

    return false;
}

}//namespace gui
