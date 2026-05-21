#include "mainwindow.h"

#include "api_client.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , gameMan()
    , mGame(std::make_unique<GameTableModel>(gameMan))
    , gameView(this)
{
    ApiClient client("https://store.steampowered.com/api/appdetails?appids=");
    std::vector<Game> games = gameMan.getAllGames();

    client.enrichGamesParallel(games);
    mGame->setGames(games);

    gameProxy = std::make_unique<QSortFilterProxyModel>();
    gameProxy->setSourceModel(mGame.get());
    gameProxy->sort(0);
    // attach model to view
    gameView.setModel(gameProxy.get());

    gameView.resizeColumnToContents(0);
    gameView.resizeColumnToContents(1);
    gameView.resizeColumnToContents(2);


    // make the table visible
    setCentralWidget(&gameView);
}

MainWindow::~MainWindow() = default;
