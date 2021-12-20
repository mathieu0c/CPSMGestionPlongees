#ifndef DIVESEARCH_H
#define DIVESEARCH_H

#include <QWidget>
#include <QSqlDatabase>

namespace Ui {
class DiveSearch;
}

namespace gui
{

class DiveSearch : public QWidget
{
    Q_OBJECT

public:
    explicit DiveSearch(QWidget *parent = nullptr);
    ~DiveSearch();

private:
    QSqlDatabase db(){
        return QSqlDatabase::database(m_dbName,false);
    }


private:
    Ui::DiveSearch *ui;

    QString m_dbName{QSqlDatabase::defaultConnection};
};

}//namespace gui

#endif // DIVESEARCH_H
