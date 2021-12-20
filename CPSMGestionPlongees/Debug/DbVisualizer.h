#ifndef DBVISUALIZER_H
#define DBVISUALIZER_H

#include <QWidget>

#include <QSqlTableModel>

namespace Ui {
class DbVisualizer;
}

namespace debug
{

class DbVisualizer : public QWidget
{
    Q_OBJECT

public:
    explicit DbVisualizer(QWidget *parent = nullptr);
    ~DbVisualizer();

    void on_pb_refreshTablesList_clicked();

    void setTableIndex(int i);

private slots:

    void on_pb_refreshTable_clicked();

    void on_cb_table_currentIndexChanged(int);

private:
    Ui::DbVisualizer *ui;

    QSqlTableModel* m_model{nullptr};
};

}

#endif // DBVISUALIZER_H
