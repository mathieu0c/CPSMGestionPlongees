#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Info/Diver.h"
#include "Info/Dive.h"

#include <QSqlQuery>

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
    void db_syncDiversWithDives(const QVector<info::Dive::MinimalDiver>& diversList);

    void diversSelected(QVector<int> idList);
    void diverChangeAccepted(info::Diver diver);
    void diverChangeRejected();

    void on_tabw_main_currentChanged(int index);

    void on_pb_newDiver_clicked();

    void on_pb_deleteDiver_clicked();

    void on_pb_editDiver_clicked();

    void divesSelected(QVector<int> idList);
    void diveChangeAccepted(info::Dive dive);
    void diveChangeRejected();

    void on_pb_deleteDive_clicked();

    void on_pb_newDive_clicked();

    void on_pb_editDive_clicked();

private:
    QSqlDatabase db(){
        return QSqlDatabase::database();
    }

private:
    Ui::MainWindow *ui;
};


}//namespace gui
#endif // MAINWINDOW_H
