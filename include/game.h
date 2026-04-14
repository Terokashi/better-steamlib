#pragma once

#include <string>
#include <unordered_set>

/**
 * @brief Represents a Steam app manifest file and its containing library.
 */
struct AppManifest {
    std::string path;          /**< Full path to the manifest file (appmanifest_XXXX.acf) */
    std::string library_path;  /**< Path to the steamapps folder containing the manifest */
};

/**
 * @brief Represents a game extracted from a manifest.
 */
struct Game {
    int appid = 0;                          /**< Steam application ID */
    std::string name;                       /**< Game name */
    std::string install_dir;                /**< Installation folder name */
    std::string library_path;               /**< Library path (drive/folder) */
    std::string manifest_path;              /**< Full path to the manifest file */
    std::unordered_set<std::string> genres; /**< Game genres */
    std::unordered_set<std::string> tags;   /**< Custom game tags */
    std::string launcher;
};