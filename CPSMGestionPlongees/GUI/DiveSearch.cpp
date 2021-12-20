#include "DiveSearch.h"
#include "ui_DiveSearch.h"

#include <QDebug>

namespace gui
{

DiveSearch::DiveSearch(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiveSearch)
{
    ui->setupUi(this);

    ui->de_endDate->setDate(QDate::currentDate());


    qDebug()<< __func__ << QDate::currentDate().isValid();
}

DiveSearch::~DiveSearch()
{
    delete ui;
}

}//namespace gui
