#include "DbVisualizer.h"
#include "ui_DbVisualizer.h"

#include "Debug/Utils.h"

#include <QSqlDatabase>

namespace debug
{

DbVisualizer::DbVisualizer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DbVisualizer)
{
    ui->setupUi(this);
}

DbVisualizer::~DbVisualizer()
{
    delete ui;
}

}//namespace debug

void debug::DbVisualizer::on_pb_refreshTablesList_clicked()
{
    ui->cb_table->clear();

    auto db{QSqlDatabase::database()};

    auto ls{db.tables()};

    for(const auto& tb : ls)
    {
        ui->cb_table->addItem(tb);
    }
}

void debug::DbVisualizer::setTableIndex(int i)
{
    ui->cb_table->setCurrentIndex(i);
}

void debug::DbVisualizer::on_pb_refreshTable_clicked()
{
    if(ui->cb_table->currentText().isEmpty())
        return;

    auto table{ui->cb_table->currentText()};

    if(!m_model)
    {
        m_model = sqlModel(QSqlDatabase::database(),table,{},this);
        ui->tableView->setModel(m_model);
    }
    else
    {
        sqlModel(m_model,table,{});
        //ui->tableView->setModel(m_model);
        if(ui->cb_table->currentIndex() == 2)
        {
            m_model->setFilter("id=1 OR id=2");
        }
    }
}

void debug::DbVisualizer::on_cb_table_currentIndexChanged(int)
{
    on_pb_refreshTable_clicked();
}
