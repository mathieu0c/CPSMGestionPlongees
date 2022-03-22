#include "DiveMembersEditor.hpp"
#include "ui_DiveMembersEditor.h"

#include "../global.hpp"

#include <QString>
#include <QLabel>
#include <QComboBox>

#include <algorithm>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "DBApi/DBApi.hpp"
#include "DBApi/DataStructs.hpp"

#include <QDebug>

namespace gui{

DiveMembersEditor::DiveMembersEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiveMembersEditor)
{
    ui->setupUi(this);

    ui->tv_divers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

DiveMembersEditor::~DiveMembersEditor()
{
    delete ui;
}

void DiveMembersEditor::setSelectionColumns(QStringList columnsNames){
    ui->tv_divers->setColumnCount(columnsNames.count());
    ui->tv_divers->setHorizontalHeaderLabels(columnsNames);
}

void DiveMembersEditor::setHiddenButton(bool hide)
{
    ui->buttonBox->setVisible(!hide);
}



void DiveMembersEditor::refreshDiverList(QSqlDatabase db,const QString& table_diverLevel)
{
    if(!m_divers)
    {
        return;
    }

//    qDebug() << "---------------- " << __func__;
//    qDebug() << m_divers->count();

    ui->tv_divers->setRowCount(m_divers->count());

    auto lvlList{db::readLFromDB<QString>(db,[&](const QSqlQuery& query)->QString{
            return query.value(0).value<QString>();
        },"SELECT level FROM %0",{table_diverLevel},{})};
    if(lvlList.size() == 0)
    {
        QString errStr{QString{"%0 : Cannot get level list"}.arg(__CURRENT_PLACE__)};
        qCritical() << errStr;
        throw std::runtime_error{errStr.toStdString()};
    }

    for(int i{}; i < m_divers->size();++i)
    {
        const auto& e = m_divers->at(i);
        ui->tv_divers->setCellWidget(i,0,new QLabel(" "+e.fullDiver.lastName+" ",ui->tv_divers));
        ui->tv_divers->setCellWidget(i,1,new QLabel(" "+e.fullDiver.firstName+" ",ui->tv_divers));
        ui->tv_divers->setCellWidget(i,2,new QLabel(" "+lvlList[e.fullDiver.diverLevelId]+" ",ui->tv_divers));

        auto cbDiveType{new QComboBox{ui->tv_divers}};
        cbDiveType->addItem(to_string(data::DiveType::exploration),i);
        cbDiveType->addItem(to_string(data::DiveType::technical),i);
        cbDiveType->setCurrentText(to_string(e.type));

        connect(cbDiveType,&QComboBox::currentIndexChanged,this,[&,cbDiveType](auto newIndex){
            auto diverIndex{cbDiveType->itemData(newIndex).toInt()};
            (*m_divers)[diverIndex].type = data::diveTypefrom_string(cbDiveType->itemText(newIndex));
        });
        ui->tv_divers->setCellWidget(i,3,cbDiveType);
    }
}

}//namespace gui
