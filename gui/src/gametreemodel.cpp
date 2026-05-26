#include <unordered_map>

#include "gametreemodel.h"


// rebuilds the tree based on change in grouping options
void GameTreeModel::rebuildTree(){
    beginResetModel();

    // creating map for grouping the games
    std::unordered_map<QString, std::vector<const Game*>> grouped;

    //reseting root
    this->m_root = nullptr;

    // group the games in grouped
    for (const Game &g : this->m_games){
        switch(this->m_mode){
            // just insert the game in one group (no grouping)
            case GroupKey::None:
                grouped["#"].push_back(&g);
                break;
            // insert the game according to it's genres (game in multiple groups)
            case GroupKey::Genre:
                for (const std::string &genre : g.genres) grouped[QString::fromStdString(genre)].push_back(&g);
                break;
            // group games according to name (gives every game it's own group)
            case GroupKey::Name: {
                QChar first = QString::fromStdString(g.name)[0].toUpper();
                QString bucket = first.isLetter() ? QString(first) : "#";
                grouped[bucket].push_back(&g);
                break;
            }
            // group games according to it's drive path
            case GroupKey::LibPath: {
                QString lib = QString::fromStdString(g.library_path)[0] + QString(":\\");
                grouped[QString::fromStdString(g.library_path)].push_back(&g);
                break;
            }
            // group games according to its full install path,
            // common (c:\\[Path to steam]\common\<game>)
            case GroupKey::InstallPath:
                grouped[QString::fromStdString(g.install_dir)].push_back(&g);
                break;
            // group games according to user tags
            case GroupKey::Tag:
                for (const std::string &t : g.tags) grouped[QString::fromStdString(t)].push_back(&g);
                break;
        }
    }


    this->m_root = std::make_unique<GameTreeNode>();

    //iterate over grouped to create nodes
    for(const auto &[label, games] : grouped) {
        std::unique_ptr<GameTreeNode> gameRoot = std::make_unique<GameTreeNode>();
        gameRoot->parent = this->m_root.get();
        gameRoot->label = label;

        // iterate over games to create leafs
        for (const Game *g : games) {
            std::unique_ptr<GameTreeNode> leaf = std::make_unique<GameTreeNode>();
            leaf->game = g;
            leaf->parent = gameRoot.get();
            gameRoot->children.push_back(std::move(leaf));
        }

        // sort the leafs
        std::sort(gameRoot->children.begin(), gameRoot->children.end(),
                  [](const auto &a, const auto &b){
            return a->game->name < b->game->name;
        });

        qDebug() << "Games in " << gameRoot->label << ": " << gameRoot->children.size();

        // assign nodes to root
        m_root->children.push_back(std::move(gameRoot));
    }

    // sort nodes in root
    std::sort(m_root->children.begin(), m_root->children.end(),
              [](const auto &a, const auto &b){
                     if (a->label == "#") return false;
                     if (b->label == "#") return true;
                     return a->label < b->label;
              });

    qDebug() << "amount of groups" << m_root->children.size();

    endResetModel();
}

int GameTreeModel::rowCount(const QModelIndex &parent) const {
    qDebug() << "Row count parent is valid: " << parent.isValid();
    if(!m_root) return 0;
    if(!parent.isValid()) return m_root->children.size();

    GameTreeNode *node = static_cast<GameTreeNode*>(parent.internalPointer());
    qDebug() << "rowCount: " << node->children.size();
    return node->children.size();
}

int GameTreeModel::columnCount(const QModelIndex &parent) const {
    return 1;
}

QModelIndex GameTreeModel::index(int row, int column, const QModelIndex &parent) const{

    if(!m_root) return QModelIndex();

    if(!parent.isValid()){
        if(row >= m_root->children.size()) return QModelIndex();
        qDebug() << "index parent invalid: " << m_root->children[row].get();
        return createIndex(row, column, m_root->children[row].get());
    }
    qDebug() << "This is row and parent is valid: " << row;

    GameTreeNode *node = static_cast<GameTreeNode*>(parent.internalPointer());
    qDebug() << "games?: " << node->game;
    qDebug() << "children size: " << node->children.size();

    if(node->children.empty()) return QModelIndex();
    if(row >= node->children.size()) return QModelIndex();

    qDebug() << "valid children: " << node->children[row].get();
    return createIndex(row, column, node->children[row].get());
}

QModelIndex GameTreeModel::parent(const QModelIndex &index) const{
    if(!index.isValid()) return QModelIndex();

    GameTreeNode *node = static_cast<GameTreeNode*>(index.internalPointer());
    if(node->parent == m_root.get()) return QModelIndex();
    if(node->parent->parent == nullptr) return QModelIndex();

    qDebug() << "parent: " << node->parent;
    qDebug() << "grandparent: " << node->parent->parent;

    for(int i = 0; i < node->parent->parent->children.size(); i++) {
        if(node->parent->parent->children[i].get() == node->parent) {
            qDebug() << "Parent Index: " << createIndex(i,0,node->parent);
            return createIndex(i, 0, node->parent);
        }
    }
    return QModelIndex();
}

QVariant GameTreeModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid()) return QVariant();
    if(role != Qt::DisplayRole) return QVariant();

    GameTreeNode *node = static_cast<GameTreeNode*>(index.internalPointer());
    if(node->game != nullptr) return QString::fromStdString(node->game->name);
    return node->label;
}
