#ifndef DIVEMEMBERSEDITOR_HPP
#define DIVEMEMBERSEDITOR_HPP

#include <QWidget>
#include <QString>

#include <memory>
#include "DBApi/DataStructs.hpp"
#include <QSqlDatabase>

#include "../global.hpp"

namespace Ui {
class DiveMembersEditor;
}

namespace gui{

class DiveMembersEditor : public QWidget
{
    Q_OBJECT

using DiverList = QVector<data::DiveMember>;

public:
    explicit DiveMembersEditor(QWidget *parent = nullptr);
    ~DiveMembersEditor();

    void setSelectionColumns(QStringList columnsNames = {});

    void setHiddenButton(bool hide);

    QVector<int> getSelectedDiversId(){
        return {};
    }

    void setDivers(DiverList& divers){
        m_divers = &divers;
    }
    void setDivers(DiverList* divers){
        m_divers = divers;
    }

public slots:
    void refreshDiverList(QSqlDatabase db, const QString &table_diverLevel);

private:
    Ui::DiveMembersEditor *ui;
    DiverList* m_divers;
};

}//namespace gui

#endif // DIVEMEMBERSEDITOR_HPP
