#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QSortFilterProxyModel>

#include "gamemanager.h"
#include "gametablemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    GameManager gameMan;
    std::unique_ptr<GameTableModel> mGame;
    QTableView gameView;
    std::unique_ptr<QSortFilterProxyModel> gameProxy;;
};
#endif // MAINWINDOW_H
