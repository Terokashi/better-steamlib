#include "cache.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

/**
 * @brief Serialize a Game object to JSON.
 *
 * @param j JSON object to populate.
 * @param g Game object to serialize.
 */
void to_json(json& j, const Game &g)
{
    j = json{
        {"appid", g.appid},
        {"name", g.name},
        {"install_dir", g.install_dir},
        {"library_path", g.library_path},
        {"manifest_path", g.manifest_path},
        {"tags", g.tags}
    };
}

/**
 * @brief Deserialize a Game object from JSON.
 *
 * @param j JSON object to read from.
 * @param g Game object to populate.
 */
void from_json(const json &j, Game &g)
{
    j.at("appid").get_to(g.appid);
    j.at("name").get_to(g.name);
    j.at("install_dir").get_to(g.install_dir);
    j.at("library_path").get_to(g.library_path);
    j.at("manifest_path").get_to(g.manifest_path);
    j.at("tags").get_to(g.tags);
}



/**
 * @brief Saves a vector of Game objects to a JSON cache file.
 *
 * Creates the parent directory if it does not exist.
 *
 * @param games Vector of Game objects to save.
 * @param path File path to save the cache (e.g., "cache/games.json").
 */
void GameCache::save(const std::vector<Game>& games)
{
    std::filesystem::create_directories(m_path.parent_path());

    json j = games;

    std::ofstream file(m_path.string());
    if (!file.is_open()) {
        std::cerr << "Failed to open cache file for writing: " << m_path << std::endl;
        return;
    }

    file << j.dump(4); // Pretty print
}


/**
 * @brief Loads a vector of Game objects from a JSON cache file.
 *
 * If the cache file does not exist or fails to parse, returns an empty vector.
 *
 * @param path Path to the cache file.
 * @return std::vector<Game> Vector of loaded Game objects.
 */
std::vector<Game> GameCache::load()
{
    std::vector<Game> games;

    std::ifstream file(m_path.string());
    if (!file.is_open()) return games;

    json j;
    try {
        file >> j;
        if (!j.is_array()) return games;
        games = j.get<std::vector<Game>>();
    } catch (const std::exception &e) {
        std::cerr << "Failed to parse cache: " << e.what() << std::endl;
    }

    return games;
}