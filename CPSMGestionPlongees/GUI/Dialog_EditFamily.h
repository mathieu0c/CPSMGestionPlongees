#ifndef GUI_DIALOG_SELECTFAMILY_H
#define GUI_DIALOG_SELECTFAMILY_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class Dialog_EditFamily;
}

namespace gui
{

class Dialog_EditFamily : public QDialog
{
    Q_OBJECT

public:
    enum class EditMode{
        cancel=-1,
        selectFamily=0,
        quitFamily=1
    };

public:
    explicit Dialog_EditFamily(QString dbName, QWidget *parent = nullptr);
    ~Dialog_EditFamily();

    EditMode mode(){
        return m_currentMode;
    }

    std::optional<int> selectedAddressId();

    void setDBName(QString name);

    void refreshDiverList();

    void setDiverId(int id){
        m_diverId = id;
    }
    void setAddressId(int id);

    void setMode(EditMode mode);



    //Return -1 if the user made the diver leave the family
    //return the address id otherwise
    //if there is no value, the action was cancelled
    static std::optional<int> getAddressId(int diverId, int addressId, QWidget* parent = nullptr, QString dbName = QSqlDatabase::defaultConnection);

public slots:
    void refreshSearchFilter(EditMode mode = EditMode::selectFamily);

private:
    QSqlDatabase db(){
        return QSqlDatabase::database(m_dbName,false);
    }

private:
    Ui::Dialog_EditFamily *ui;

    QString m_dbName{QSqlDatabase::defaultConnection};

    int m_diverId{};
    int m_addressId{};

    EditMode m_currentMode{};
};

}//namespace gui

#endif // GUI_DIALOG_SELECTFAMILY_H
