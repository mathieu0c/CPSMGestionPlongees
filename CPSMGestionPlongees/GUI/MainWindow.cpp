#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "GUI/constSettings.hpp"

#include "GUI/DiverEdit.h"
#include "GUI/Dialog_ConfirmDiverDeletion.h"

#include "../global.hpp"
#include "GUI/global.hpp"

#include "Info/Diver.h"
#include "Info/Dive.h"
#include "Info/Address.h"
#include "Info/global.hpp"
#include "Info/Generic.hpp"

#include <QFile>

#include "Data/Database.hpp"
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>

#include "Debug/LogFile.hpp"
#include "Debug/DbVisualizer.h"
#include "Debug/Utils.h"

namespace gui
{

void setEnableDebug(bool state,bool enableLog = false){
    db::enableDebug = state;
    gui::enableDebug = state;
    info::enableDebug = state;

    if(enableLog)
    {
        static bool firstCall{true};
        if(firstCall)
        {
            debug::startLog();
            firstCall = false;
        }
    }
    debug::enable_logFile = enableLog;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setEnableDebug(false,true);

    auto openedDB{db::openLocal("d")};
    if(!openedDB)
        throw std::runtime_error("Cannot open DB");

    db::createDB();
    db::initDB();

    ui->tab_debug->on_pb_refreshTablesList_clicked();
    ui->tab_debug->setTableIndex(1);

    ui->pg_editDiver->refreshLevelList(db::getDiverLevels());
    ui->pg_editDive->refreshSiteList(db::getDiveSites());

    connect(ui->mainDiverSearch,&gui::DiverSearch::diversSelected,this,&MainWindow::diversSelected);

    connect(ui->pg_editDiver,&gui::DiverEdit::endEditing,this,&MainWindow::diverChangeAccepted);
    connect(ui->pg_editDiver,&gui::DiverEdit::rejectedEditing,this,&MainWindow::diverChangeRejected);


    connect(ui->mainDiveSearch,&gui::DiveSearch::divesSelected,this,&MainWindow::divesSelected);

    connect(ui->pg_editDive,&gui::DiveEdit::endEditing,this,&MainWindow::diveChangeAccepted);
    connect(ui->pg_editDive,&gui::DiveEdit::rejectedEditing,this,&MainWindow::diveChangeRejected);

    ui->mainDiverSearch->setHiddenButton(true);
    ui->mainDiverSearch->refreshDiverList();

    ui->mainDiveSearch->setHiddenButton(true);
    ui->mainDiveSearch->refreshDivesList();
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_tabw_main_currentChanged(int i)
{
    if(ui->tabw_main->tabText(i) == "Plongeurs")
    {
        ui->mainDiverSearch->refreshDiverList();
    }
}

void MainWindow::diversSelected(QVector<int> idList)
{
    qDebug() << "   DIVERS IDs : " << idList;
    if(idList.size() == 0)
        return;

    auto id{idList[0]};
    auto tempDiver{info::readDiverFromDB(id,QSqlDatabase::database(),global::table_divers)};

    ui->pg_editDiver->setDiver(std::move(tempDiver));
    ui->tab_divers->setCurrentIndex(1);
}

void MainWindow::diverChangeAccepted(info::Diver diver)
{
    info::storeInDB(diver,QSqlDatabase::database(),global::table_divers);

    ui->tab_divers->setCurrentIndex(0);
    ui->mainDiverSearch->refreshDiverList();
}

void MainWindow::diverChangeRejected()
{
    ui->tab_divers->setCurrentIndex(0);
    ui->mainDiverSearch->refreshDiverList();
}


void MainWindow::on_pb_deleteDiver_clicked()
{
    auto diversIds{ui->mainDiverSearch->getSelectedDiversId()};
    if(diversIds.size() == 0)
        return;

    QVector<QVariant> idList{};
    idList.resize(diversIds.size());

    QString querySelectStr{"SELECT lastName,firstName FROM %0 WHERE "};
    for(int i{}; i < diversIds.size(); ++i)
    {
        idList[i] = diversIds[i];
        querySelectStr += "id=?";
        if(i < diversIds.size()-1)
            querySelectStr += " OR ";
    }
//    qDebug() << querySelectStr;
    auto db{QSqlDatabase::database()};
    auto divers{db::querySelect(db,querySelectStr+" ORDER BY lastName",{global::table_divers},idList)};

    QStringList diverList{};

    for(const auto& e : divers)
    {
        diverList += e[0].value<QString>() + " " + e[1].value<QString>();
    }

    auto confirmed{Dialog_ConfirmDiverDeletion::confirmDeletion(diverList,this)};

    if(!confirmed)
        return;

    for(const auto& id : idList)
    {
        info::removeAllFromDiver(id.toInt(),db,global::table_divers);//delete the diver from the DB
    }

    ui->mainDiverSearch->refreshDiverList();
}


void MainWindow::on_pb_editDiver_clicked()
{
    auto diversIds{ui->mainDiverSearch->getSelectedDiversId()};
    diversSelected(diversIds);
}

void MainWindow::on_pb_newDiver_clicked()
{
    ui->pg_editDiver->resetDiver();

    ui->tab_divers->setCurrentIndex(1);
}

void MainWindow::divesSelected(QVector<int> idList)
{
//    qDebug() << "------------- " << __func__ << " -------------";
//    qDebug() << idList;

    if(idList.size() == 0)
        return;

    auto id{idList[0]};
    auto tempDive{info::readDiveFromDB(id,QSqlDatabase::database(),global::table_dives)};

//    ui->pg_editDiver->setDiver(std::move(tempDiver));
//    ui->tab_divers->setCurrentIndex(1);
    ui->pg_editDive->setDive(tempDive);
    ui->pg_editDive->refreshDiversList();
    ui->tab_dives->setCurrentIndex(0);//switch to edit page
}

void MainWindow::diveChangeAccepted(info::Dive dive)
{
//    info::storeInDB(diver,QSqlDatabase::database(),global::table_divers);

    qDebug() << "Dive accepted : " << dive;
    ui->tab_dives->setCurrentIndex(1);//switch to search dive page
    ui->mainDiveSearch->refreshDivesList();
}

void MainWindow::diveChangeRejected()
{
    ui->tab_dives->setCurrentIndex(1);//switch to search dive page
    ui->mainDiveSearch->refreshDivesList();
}

}//namespace gui
