#ifndef DIVEMEMBERSEDITOR_HPP
#define DIVEMEMBERSEDITOR_HPP

#include <QWidget>
#include <QString>

#include <memory>
#include "DBApi/DataStructs.hpp"
#include "DBApi/DBApi.hpp"
#include <unordered_map>
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

    void setEditable(bool enable);

    void setDivers(DiverList& divers){
        m_divers = &divers;
    }
    void setDivers(DiverList* divers){
        m_divers = divers;
    }

    void setDiverLevelList(const QVector<data::DiverLevel>& lvlList);

    QVector<int> selectedDiversId() const;

    inline QVector<int> diversIds()const;
    QString formattedDiversIds()const;

public slots:
    void refreshDiverList(const QVector<data::DiverLevel>& lvlList,bool sortBefore = true);
    void refreshDiverList(bool sortBefore = true);
    void applyFilter(const QString& toContains);

signals:
    void doubleClick();

private:
    Ui::DiveMembersEditor *ui;
    DiverList* m_divers;
    bool m_isEditable;
    std::unordered_map<int,data::DiverLevel> m_diverLevelIdIndexMap;
};

inline
void DiveMembersEditor::setDiverLevelList(const QVector<data::DiverLevel>& lvlList)
{
    m_diverLevelIdIndexMap.clear();
    m_diverLevelIdIndexMap.reserve(lvlList.size());
    for(const auto& lvl : lvlList)
    {
        m_diverLevelIdIndexMap[lvl.id] = lvl;
    }
}

inline
QVector<int> DiveMembersEditor::diversIds()const {
    if(!m_divers)
        return {};
    QVector<int> out;
    out.reserve(m_divers->size());
    for(const auto& e : *m_divers)
    {
        out.append(e.diverId);
    }
    return out;
}

inline
QString DiveMembersEditor::formattedDiversIds() const {
    return db::formatListForSQL(diversIds());
}

}//namespace gui

#endif // DIVEMEMBERSEDITOR_HPP
