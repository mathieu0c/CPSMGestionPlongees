#include "Dialog_ConfirmDiveDeletion.hpp"
#include "ui_Dialog_ConfirmDiveDeletion.h"

#include "GUI/global.hpp"

namespace gui
{

Dialog_ConfirmDiveDeletion::Dialog_ConfirmDiveDeletion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_ConfirmDiveDeletion)
{
    ui->setupUi(this);
}

Dialog_ConfirmDiveDeletion::~Dialog_ConfirmDiveDeletion()
{
    delete ui;
}

void Dialog_ConfirmDiveDeletion::setDiverList(const QStringList& diverList)
{
    for(const auto& e : diverList)
    {
        ui->lw_diveList->addItem(e);
    }
}

bool Dialog_ConfirmDiveDeletion::confirmDeletion(const QStringList& diverList,QWidget* parent)
{
    auto dial{Dialog_ConfirmDiveDeletion(parent)};
    dial.ui->lw_diveList->addItems(diverList);

    auto ans{dial.exec()};

    if(ans == QDialog::Accepted)
    {
        if(enableDebug)
        {
            qDebug() <<"Dialog_ConfirmDiveDeletion::" << __func__ <<" : Confirmed deletion of " << diverList;
        }

        return true;
    }//End if

    return false;
}

}//namespace gui
