#ifndef GUI_DIVEREDIT_H
#define GUI_DIVEREDIT_H

#include <QWidget>
#include <QStringList>

//#include "DataStruct/Diver.h"
#include "DBApi/DataStructs.hpp"

#include "GUI/Dialog_EditFamily.h"

namespace Ui {
class DiverEdit;
}

namespace gui
{

class DiverEdit : public QWidget
{
    Q_OBJECT

public:
    explicit DiverEdit(QWidget *parent = nullptr);
    ~DiverEdit();

    void refreshLevelList(QVector<data::DiverLevel> levels, bool sortByAlphabetical = true);

    const data::Diver& diver() const{
        return m_tempDiver;
    }
    data::Diver diver(){
        return m_tempDiver;
    }

    void setAddress(data::Address address);
    void setDiver(data::Diver diver);

    void computeDivingCount();

    void resetDiver();

signals:
    void endEditing(const data::Diver& diver);
    void rejectedEditing();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_cb_gear_global_stateChanged(int arg1);

    void on_pb_family_clicked();

private:
    Ui::DiverEdit *ui;

    data::Diver m_tempDiver{};
    int m_tempDiverOriginalPaidDives{};
};

}//namespace gui

#endif // GUI_DIVEREDIT_H
