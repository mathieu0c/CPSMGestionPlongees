#ifndef GUI_DIALOG_CONFIRMDIVERDELETION_H
#define GUI_DIALOG_CONFIRMDIVERDELETION_H

#include <QDialog>

namespace Ui {
class Dialog_ConfirmDiverDeletion;
}

namespace gui
{

class Dialog_ConfirmDiverDeletion : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_ConfirmDiverDeletion(QWidget *parent = nullptr);
    ~Dialog_ConfirmDiverDeletion();

    void setDiverList(const QStringList& diverList);

    static bool confirmDeletion(const QStringList& diverList,QWidget* parent = nullptr);

private:
    Ui::Dialog_ConfirmDiverDeletion *ui;
};

}//namespace gui

#endif // GUI_DIALOG_CONFIRMDIVERDELETION_H
