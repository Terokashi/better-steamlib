#pragma once

#include <vector>
#include <filesystem>
#include "game.h"

class GameCache {
private:
    std::filesystem::path m_path;
public:
    GameCache
        (const std::filesystem::path m_path = std::filesystem::path(("cache/steam_games.json")))
        : m_path(m_path){};
    /**
    * @brief Saves a list of Game objects to a cache file.
    *
    * This function serializes the provided vector of Game objects and writes
    * them to the specified file path.
    *
    * @param games The vector of Game objects to save.
    * @param path The file system path where the cache file will be written.
    *
    * @throws std::runtime_error If the file cannot be written.
    */
    void save(const std::vector<Game>& games);

    /**
     * @brief Loads a list of Game objects from a cache file.
     *
     * This function reads serialized Game data from the specified file path
     * and reconstructs a vector of Game objects.
     *
     * @param path The file system path to the cache file.
     * @return A std::vector<Game> containing all loaded Game objects.
     *
     * @throws std::runtime_error If the file cannot be opened or parsing fails.
     */
    std::vector<Game> load();
};
