#pragma once

#include "igamelibrary.h"
#include <memory>

class GameManager
{
private:
    std::vector<std::unique_ptr<IGameLibrary>> libraries;
public:
    GameManager();

    void refresh();
    void addLibrary(std::unique_ptr<IGameLibrary> lib);
    std::vector<Game> getAllGames();
    static std::string parseLauncher(const Launcher &launcher);
};

