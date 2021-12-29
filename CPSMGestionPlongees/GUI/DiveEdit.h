#ifndef GUI_DIVEEDIT_H
#define GUI_DIVEEDIT_H

#include <QWidget>
#include <QStringList>

#include "Info/Dive.h"


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

    void refreshDiversList();
    void refreshDiverSearchFilters_global();
    void refreshDiverSearchFilters_dive();

    const info::Dive& dive() const{
        return m_tempDive;
    }
    info::Dive dive(){
        return m_tempDive;
    }

    void setDive(info::Dive dive);

    void resetDive();


signals:
    void endEditing(const info::Dive& dive);
    void rejectedEditing();

private slots:
    void slot_diveComboBox(int index);
    void refreshDiversListComboBox();//recreate the combo box for dive type

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

//    void on_cb_gear_global_stateChanged(int arg1);

    void on_pb_diverToDive_clicked();

    void on_pb_diveToDiver_clicked();

private:
    Ui::DiveEdit *ui;

    info::Dive m_tempDive{};
};

}//namespace gui

#endif // GUI_DIVEEDIT_H
