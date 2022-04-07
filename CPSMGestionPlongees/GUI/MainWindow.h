#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

//#include "DataStruct/Diver.h"
//#include "DataStruct/Dive.h"
#include "DBApi/DataStructs.hpp"

#include <QSqlQuery>

#include "Update/UpdateManager.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


namespace gui
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void diversSelected(QVector<int> idList);
    void diverChangeAccepted(data::Diver diver);
    void diverChangeRejected();

    void on_tabw_main_currentChanged(int index);

    void on_pb_newDiver_clicked();

    void on_pb_deleteDiver_clicked();

    void on_pb_editDiver_clicked();

    void divesSelected(QVector<int> idList);
    void diveChangeAccepted(data::Dive dive);
    void diveChangeRejected();

    void on_pb_deleteDive_clicked();

    void on_pb_newDive_clicked();

    void on_pb_editDive_clicked();

private:
    void initSettings();

    QSqlDatabase db(){
        return QSqlDatabase::database();
    }

private:
    Ui::MainWindow *ui;
    updt::UpdateManager m_updateManager;
    bool m_wasUpdated;
};


}//namespace gui
#endif // MAINWINDOW_H
