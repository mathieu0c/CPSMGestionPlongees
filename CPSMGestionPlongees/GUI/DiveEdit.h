#ifndef GUI_DIVEEDIT_H
#define GUI_DIVEEDIT_H

#include <QWidget>
#include <QStringList>

//#include "DataStruct/Dive.h"
#include "DBApi/DataStructs.hpp"

#include "../global.hpp"

#include <QDialog>
#include <QDialogButtonBox>

namespace Ui {
class DiveEdit;
}

namespace gui
{

class DiveEdit : public QWidget
{
    Q_OBJECT

public:
    explicit DiveEdit(QWidget *parent = nullptr);
    ~DiveEdit();

    void refreshSiteList(const QString &siteTable);

    void setEditable(bool isEditable);

    void refreshDiversList();
    void refreshDiverSearchFilters_global();
    void refreshDiverSearchFilters_dive();

    const data::Dive& dive() const{
        return m_tempDive;
    }
    data::Dive dive(){
        return m_tempDive;
    }

    void setDive(data::Dive dive);

    void resetDive();

    void setDiverLevelList(const QVector<data::DiverLevel> &lvlList);

public:
static void displayDive(const data::Dive& dive,QWidget* parent);

signals:
    void endEditing(const data::Dive& dive);
    void rejectedEditing();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

//    void on_cb_gear_global_stateChanged(int arg1);

    void on_pb_diverToDive_clicked();

    void on_pb_diveToDiver_clicked();

private:
    Ui::DiveEdit *ui;

    bool m_isEditable{true};
    data::Dive m_tempDive{};
};

}//namespace gui

#endif // GUI_DIVEEDIT_H
