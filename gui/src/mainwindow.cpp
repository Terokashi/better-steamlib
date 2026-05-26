#include "mainwindow.h"

#include "api_client.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , gameMan()
    , mGame()
    , gameView(this)
{
    ApiClient client("https://store.steampowered.com/api/appdetails?appids=");
    gameMan.refresh();
    std::vector<Game> games = gameMan.getAllGames();
    client.enrichGamesParallel(games);
    mGame = std::make_unique<GameTreeModel>(games);

    QModelIndex groupIndex = mGame->index(0, 0, QModelIndex());
    qDebug() << "group index valid: " << groupIndex.isValid();
    qDebug() << "group parent: " << mGame->parent(groupIndex);
    qDebug() << "round trip holds: " << (mGame->parent(groupIndex) == QModelIndex());

    qDebug() << "main amount of games: " << games.size();

    // attach model to view
    gameView.setModel(mGame.get());

    gameView.resizeColumnToContents(0);

    // make the table visible
    setCentralWidget(&gameView);
}

MainWindow::~MainWindow() = default;
