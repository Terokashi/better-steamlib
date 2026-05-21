#include "gamemanager.h"
#include "steam_parser.h"

GameManager::GameManager() {
    addLibrary(std::make_unique<SteamLibrary>());
}

void GameManager::refresh() {
    for(const auto &lib : libraries){
        lib->refresh();
    }
}

void GameManager::addLibrary(std::unique_ptr<IGameLibrary> lib) {
    libraries.push_back(std::move(lib));
}
std::vector<Game> GameManager::getAllGames() {
    std::vector<Game> result;

    for(const auto &lib : libraries){
        std::vector<Game> games = lib->loadGames();
        result.insert(result.end(), games.begin(), games.end());
    }
    return result;
}

std::string GameManager::parseLauncher(const Launcher &launcher){
    switch(launcher) {
    case Launcher::Steam:
        return "Steam";
    case Launcher::Epic:
        return "Epic";
    case Launcher::GoG:
        return "GoG";
    default:
        return "None";
    }
}