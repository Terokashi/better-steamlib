#pragma once

#include <string>
#include <vector>

#include "igamelibrary.h"
#include "cache.h"

class SteamLibrary : public IGameLibrary {
private:
    GameCache m_cache{"cache/steam_games.json"};
    std::string library_path = R"(C:\Program Files (x86)\Steam\steamapps\libraryfolders.vdf)";

    /**
     * @brief Unescapes special characters in a list of paths.
     *
     * This function processes each path and replaces escaped sequences
     * (e.g., "\n", "\\") with their literal equivalents.
     *
     * @param paths The list of paths to unescape.
     * @return A vector of unescaped paths.
     */
    std::vector<std::string> unescapePaths(const std::vector<std::string> &paths);

    /**
     * @brief Parses a manifest key from content.
     *
     * Extracts the value associated with a specific key from a manifest-style
     * content string.
     *
     * @param content The manifest content to parse.
     * @param key The key whose value should be extracted.
     * @return The value corresponding to the key.
     *
     * @throws std::invalid_argument If the key is not found in the content.
     */
    std::string parseManifest(const std::string &content, const std::string &key);

public:
    /**
     * @brief Extracts paths from a content string using a specified key.
     *
     * This function searches the provided content for entries matching the key
     * and returns the associated paths.
     *
     * @param content The string content to search within.
     * @param key The key to search for in the content.
     * @return A vector of strings representing the extracted paths.
     */
    std::vector<std::string> getPaths(const std::string &content, const std::string &key);

    /**
     * @brief Constructs AppManifest objects from a list of paths.
     *
     * This function iterates over each path, reads and parses manifest content,
     * and returns a list of AppManifest objects.
     *
     * @param paths A vector of file paths to manifest files.
     * @param apps Optional parameter specifying which apps to include.
     * @return A vector of AppManifest objects.
     */
    std::vector<AppManifest> getManifests(const std::vector<std::string> &paths, const std::string &apps);

    /**
     * @brief Reads the content of a file into a string.
     *
     * @param path The file system path to read from.
     * @return The content of the file as a string.
     *
     * @throws std::runtime_error If the file cannot be opened or read.
     */
    std::string getContent(const std::string &path);

    /**
     * @brief Converts AppManifest objects into Game objects.
     *
     * This function transforms the data from AppManifest structures into
     * Game objects suitable for further processing in the application.
     *
     * @param manifests A vector of AppManifest objects to convert.
     * @return A vector of Game objects.
     */
    std::vector<Game> getGames(const std::vector<AppManifest> &manifests);

    void refresh();
    std::vector<Game> loadGames();
};



