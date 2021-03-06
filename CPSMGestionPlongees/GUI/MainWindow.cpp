#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "GUI/constSettings.hpp"

#include "Update/UpdateManager.hpp"
#include "Update/PostUpdate.hpp"

#include "GUI/DiverEdit.h"
#include "GUI/Dialog_ConfirmDiverDeletion.h"
#include "GUI/Dialog_ConfirmDiveDeletion.hpp"
#include <QMessageBox>

#include "../global.hpp"
#include "GUI/global.hpp"


//#include "DataStruct/Diver.h"
//#include "DataStruct/Dive.h"
//#include "DataStruct/Address.h"
//#include "DataStruct/global.hpp"
#include "DBApi/DataStructs.hpp"

#include <QFile>

#include "DBApi/Database.hpp"
#include "DBApi/Generic.hpp"
#include "DBApi/DBDiverLevel.hpp"
#include "DBApi/DBApi.hpp"
#include "DBApi/DBSaver.hpp"
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>

#include "Debug/LogFile.hpp"
#include "Debug/DbVisualizer.h"
#include "Debug/Utils.h"

#include "GenericSettings/GenericSettings.hpp"
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>

namespace gui
{

void setEnableDebug(bool state,bool enableLog = false){
    db::enableDebug = state;
    gui::enableDebug = state;
    data::enableDebug = state;

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

//QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)

void MainWindow::initSettings()
{
    using namespace gens;
    using namespace global;

    if(!QFileInfo::exists(settings_dataDir))//if the appdata folder doesn't exist
    {
        if(!QDir::root().mkpath(settings_dataDir))
        {
            throw std::runtime_error{__CURRENT_PLACE__.toStdString() + " : Cannot create appData folder : <" + settings_dataDir.toStdString() + ">"};
        }
    }

    db::DBSaverInfo dbSaver{};

    SettingsManager::addSetting("DBSaverInfo",dbSaver,db::dbSaverInfoToJson,db::dbSaverInfoFromJson);
    SettingsManager::addSetting(SK::dbPath,global::settings_dataDir+"cpsm.sqlite3");
    SettingsManager::addSetting(SK::lastTab,ui->tabw_main->currentIndex());

    SettingsManager::addSetting(SK::sortLevelsByAlphabetical,true);

    SettingsManager::read(settings_confFile,true);//read config file if exists

    ui->tabw_main->setCurrentIndex(SettingsManager::ref<int>(SK::lastTab));
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    m_updateManager{false,this},
    m_wasUpdated{updt::postUpdateFunction()}
{
    ui->setupUi(this);

    constexpr auto enableDebug{false};
    setEnableDebug(enableDebug,true);


    initSettings();
    auto& dbSaver{gens::SettingsManager::ref<db::DBSaverInfo>("DBSaverInfo")};
    db::refreshSave(dbSaver);

    auto dbAlreadyExist{QFileInfo::exists(gens::SettingsManager::ref<QString>(global::SK::dbPath))};

    auto openedDB{db::openLocal(gens::SettingsManager::ref<QString>(global::SK::dbPath))};
    if(!openedDB)
        throw std::runtime_error("Cannot open DB");

    qDebug() << "CREATE DB ?? : " << !dbAlreadyExist;
    if(!dbAlreadyExist)
    {
#ifdef PROJECT_PREVIEW
        db::initDB();
#else
        db::createDB();
#endif
    }

    auto db{this->db()};

    ui->tab_debug->on_pb_refreshTablesList_clicked();
    ui->tab_debug->setTableIndex(1);


    auto lvlList{db::readLFromDB<data::DiverLevel>
                (db,db::extractDiverLevelFromQuery,"SELECT * FROM %1",
                 {global::table_diverLevel},{})};
    ui->pg_editDiver->refreshLevelList(lvlList,
                                       gens::SettingsManager::ref<bool>(global::SK::sortLevelsByAlphabetical));
    ui->pg_editDive->setDiverLevelList(lvlList);
    ui->pg_editDive->refreshSiteList(global::table_divingSites);

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

    //UPDATES
//    connect(ui->action_checkUpdates,&QAction::triggered,&m_updateManager,&updt::UpdateManager::checkUpdate);

    connect(&m_updateManager,&updt::UpdateManager::hiddenUpdateAvailable,this,[&](){
        auto ans = QMessageBox::question(this,tr("Update available"),tr("A new update is available. Do you want to download it ?"));
        if(ans == QMessageBox::Yes)
        {
            m_updateManager.exec();
        }
    });
    connect(&m_updateManager,&updt::UpdateManager::hiddenNoUpdateAvailable,this,[&](){
        ui->statusbar->showMessage(tr("No available update found"),10000);
    });
    m_updateManager.checkUpdate();

// -- -- -- -- -- -- debugging purpose
#ifdef COMPILED_FOR_RELEASE
#ifdef PROJECT_PREVIEW
    QMessageBox::critical(this,"Rappel","C'est une version de test. Elle n'est probablement pas stable et cela pourrait r??sulter en des pertes de donn??es");
    ui->tabw_main->setTabVisible(0,false);
#endif
#endif

//    ui->pg_editDive->setEditable(false);
    auto existingDivesIds{db::querySelect(db,"SELECT id FROM %0",{global::table_dives},{})};
    QVector<data::Dive> existingDives{existingDivesIds.size()};
    for(const auto& dbLine : existingDivesIds)
    {
        auto dive{db::readDiveFromDB(dbLine[0].toInt(),db,global::table_dives,global::table_divingSites,
                                    global::table_divesMembers,global::table_divers)};
//        db_syncDiversWithDives(dive.divers);
    }

//    auto results{db::readLFromDB<int>(db,[&](const QSqlQuery& q)->int{return q.value(0).value<int>();},"SELECT id FROM %0",{global::table_dives},{})};
//    for(const auto& e : results)
//    {
//        qDebug() << __CURRENT_PLACE__ <<e;
//    }

    data::Address adrTest{1,"BIDULE","TRUC","JSP"};
    db::storeInDB(adrTest,db,global::table_diversAddresses);
}

MainWindow::~MainWindow()
{
    qDebug() <<"Saving settings as : "<< global::settings_confFile;
    gens::SettingsManager::saveAs(global::settings_confFile);

    delete ui;
}



void MainWindow::on_tabw_main_currentChanged(int i)
{
    gens::SettingsManager::ref<int>(global::SK::lastTab) = i;

    if(ui->tabw_main->tabText(i) == "Plongeurs")
    {
        ui->mainDiverSearch->refreshDiverList();
        diveChangeRejected();
    }
    else
    {
        ui->mainDiveSearch->refreshDivesList();
        diverChangeRejected();
    }
}


void MainWindow::diversSelected(QVector<int> idList)
{
//    qDebug() << "   DIVERS IDs : " << idList;
    if(idList.size() == 0)
        return;

    auto id{idList[0]};
    auto tempDiver{db::readDiverFromDB(id,db(),global::table_divers,global::table_divesMembers)};

    ui->pg_editDiver->setDiver(std::move(tempDiver));

    ui->tab_divers->setCurrentIndex(1);
}

void MainWindow::diverChangeAccepted(data::Diver diver)
{
    using db::storeInDB;
    storeInDB(diver,db(),global::table_divers);

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
    auto db{this->db()};
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
        db::removeAllFromDiver(id.toInt(),db,global::table_divers);//delete the diver from the DB
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
    auto tempDive{db::readDiveFromDB(id,db(),global::table_dives,global::table_divingSites,
                                     global::table_divesMembers,global::table_divers)};

//    ui->pg_editDiver->setDiver(std::move(tempDiver));
//    ui->tab_divers->setCurrentIndex(1);
    ui->pg_editDive->setDive(tempDive);
    ui->pg_editDive->refreshDiversList();
    ui->tab_dives->setCurrentIndex(0);//switch to edit page
}

void MainWindow::diveChangeAccepted(data::Dive dive)
{
    using db::storeInDB;
    storeInDB(dive,db(),global::table_dives,global::table_divers,global::table_divesMembers);

//    qDebug() << "Dive accepted : " << dive;
    ui->tab_dives->setCurrentIndex(1);//switch to search dive page
    ui->mainDiveSearch->refreshDivesList();

//    db_syncDiversWithDives(dive.divers);
}

void MainWindow::diveChangeRejected()
{
    ui->tab_dives->setCurrentIndex(1);//switch to search dive page
    ui->mainDiveSearch->refreshDivesList();
}


void MainWindow::on_pb_deleteDive_clicked()
{
    auto divesIds{ui->mainDiveSearch->getSelectedDivesId()};
    if(!divesIds.size())
        return;

    QVector<data::Dive> diveList{};
    diveList.reserve(divesIds.size());

    QStringList diveListConfirmation{diveList.size()};

    auto db{this->db()};

    for(const auto& id : divesIds)
    {
        auto dive{db::readDiveFromDB(id,db,global::table_dives,global::table_divingSites,
                                     global::table_divesMembers,global::table_divers)};
        auto dbDiveSite{db::querySelect(db,"SELECT name FROM %0 WHERE id=?",{global::table_divingSites},{dive.diveSiteId})};
        diveListConfirmation.append(QString{"%0 - %1 (%2 "}.arg(dive.date.toString(global::format_date),
                                                                dbDiveSite[0][0].toString()).arg(dive.diver .size())+tr("plongeur(s)")+")");
        diveList.append(std::move(dive));
    }


    auto confirmed{gui::Dialog_ConfirmDiveDeletion::confirmDeletion(diveListConfirmation,this)};

    if(!confirmed)
        return;

    for(const auto& dive : diveList)
    {
        auto success{db::removeAllFromDB(dive,db,global::table_dives)};
        if(!success)
            ui->statusbar->showMessage(tr("Impossible de supprimer les plong??es s??lectionn??es"));

//        db_syncDiversWithDives(dive.divers);
    }

    ui->mainDiveSearch->refreshDivesList();
}


void MainWindow::on_pb_newDive_clicked()
{
    ui->pg_editDive->resetDive();

    ui->tab_dives->setCurrentIndex(0);//switch to edit dive tab
}


void MainWindow::on_pb_editDive_clicked()
{
    auto divesIds{ui->mainDiveSearch->getSelectedDivesId()};
    divesSelected(divesIds);
}

}//namespace gui
