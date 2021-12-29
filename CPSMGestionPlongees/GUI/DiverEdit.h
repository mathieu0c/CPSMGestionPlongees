#ifndef GUI_DIVEREDIT_H
#define GUI_DIVEREDIT_H

#include <QWidget>
#include <QStringList>

#include "Info/Diver.h"

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

    void refreshLevelList(const QStringList& list);

    const info::Diver& diver() const{
        return m_tempDiver;
    }
    info::Diver diver(){
        return m_tempDiver;
    }

    void setAddress(info::Address address);
    void setDiver(info::Diver diver);

    void computeDivingCount();

    void resetDiver();

signals:
    void endEditing(const info::Diver& diver);
    void rejectedEditing();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_cb_gear_global_stateChanged(int arg1);

    void on_pb_family_clicked();

private:
    Ui::DiverEdit *ui;

    info::Diver m_tempDiver{};
    int m_tempDiverOriginalPaidDives{};
};

}//namespace gui

#endif // GUI_DIVEREDIT_H
