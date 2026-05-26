#pragma once

#include <qabstractItemModel>

#include "game.h"

struct GameTreeNode {
    QString label;
    const Game *game = nullptr;

    std::vector<std::unique_ptr<GameTreeNode>> children;
    GameTreeNode *parent = nullptr;
};

class GameTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    std::vector<Game> m_games;

    GroupKey m_mode = GroupKey::Genre;
    std::unique_ptr<GameTreeNode> m_root;
public:
    GameTreeModel(const std::vector<Game> &games) : m_games(games){
        rebuildTree();
    };

    void rebuildTree();
    void setGrouping(GroupKey mode);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;
};
