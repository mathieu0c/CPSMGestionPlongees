#ifndef GUI_DIALOG_CONFIRMDIVEDELETION_HPP
#define GUI_DIALOG_CONFIRMDIVEDELETION_HPP

#include <QDialog>

namespace Ui {
class Dialog_ConfirmDiveDeletion;
}

namespace gui
{

class Dialog_ConfirmDiveDeletion : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_ConfirmDiveDeletion(QWidget *parent = nullptr);
    ~Dialog_ConfirmDiveDeletion();

    void setDiverList(const QStringList& diveList);

    static bool confirmDeletion(const QStringList& diveList,QWidget* parent = nullptr);

private:
    Ui::Dialog_ConfirmDiveDeletion *ui;
};

}//namespace gui

#endif // GUI_DIALOG_CONFIRMDIVEDELETION_HPP
