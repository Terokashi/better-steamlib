#include "steam_parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

/**
 * @brief Reads the entire content of a file into a string.
 *
 * @param path Path to the file.
 * @return File content as a std::string. Returns empty string if the file cannot be opened.
 */
std::string SteamLibrary::getContent(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

/**
 * @brief Extracts a value associated with a key from a Steam appmanifest VDF content.
 *
 * Searches for keys like "name", "appid", or "installdir" and returns their corresponding value.
 *
 * @param content The manifest content as a string.
 * @param key The key to search for (e.g., "name", "appid", "installdir").
 * @return The value as a string, or empty if the key is not found.
 */
std::string SteamLibrary::parseManifest(const std::string &content, const std::string &key) {
    size_t keyPos = content.find("\"" + key + "\"");
    if (keyPos == std::string::npos) return "";

    size_t start_quote = content.find("\"", keyPos + key.length() + 2);
    if (start_quote == std::string::npos) return "";

    size_t end_quote = content.find("\"", start_quote + 1);
    if (end_quote == std::string::npos) return "";

    return content.substr(start_quote + 1, end_quote - start_quote - 1);
}

/**
 * @brief Converts escaped Steam paths (double backslashes) into normal Windows paths.
 *
 * Steam VDF files escape backslashes by doubling them.
 *
 * @param escape_paths Vector of escaped paths from VDF files.
 * @return Vector of unescaped paths.
 */
std::vector<std::string> SteamLibrary::unescapePaths(const std::vector<std::string> &escape_paths) {
    std::vector<std::string> results;

    for (const auto &escape_path : escape_paths) {
        std::string result;
        for (size_t i = 0; i < escape_path.size(); i++) {
            if (escape_path[i] == '\\' && i + 1 < escape_path.size() && escape_path[i + 1] == '\\') {
                result += '\\';
                i++;
            } else {
                result += escape_path[i];
            }
        }
        results.push_back(result);
    }

    return results;
}

/**
 * @brief Extracts all occurrences of a key in a text block (VDF content).
 *
 * Example: Extracts all "path" values from libraryfolders.vdf.
 *
 * @param content Text content to search.
 * @param find The key to find (e.g., "path").
 * @return Vector of values associated with the key.
 */
std::vector<std::string> SteamLibrary::getPaths(const std::string &content, const std::string &find) {
    std::vector<std::string> results;
    size_t pos = 0;

    while ((pos = content.find(find, pos)) != std::string::npos) {
        size_t start = content.find("\"", pos + find.length() + 2);
        if (start == std::string::npos) break;

        size_t end = content.find("\"", start + 1);
        if (end == std::string::npos) break;

        results.push_back(content.substr(start + 1, end - start - 1));
        pos = end;
    }

    return results;
}

/**
 * @brief Scans Steam library folders for appmanifest_*.acf files.
 *
 * Each library path is scanned for the "steamapps" folder, and all
 * appmanifest files are collected.
 *
 * @param library_paths Vector of unescaped Steam library paths.
 * @param apps Subfolder containing manifests (typically "\\steamapps").
 * @return Vector of AppManifest objects containing library path and manifest path.
 */
std::vector<AppManifest> SteamLibrary::getManifests(const std::vector<std::string> &library_paths, const std::string &apps) {
    std::vector<AppManifest> manifests;

    for (const auto &path : library_paths) {
        std::string fullPath = path + apps;

        if (!std::filesystem::exists(fullPath)) continue;

        for (const auto &it : std::filesystem::directory_iterator(fullPath)) {
            std::string fname = it.path().filename().string();
            if (fname.find("appmanifest_") != 0 || it.path().extension() != ".acf") continue;

            AppManifest tmp;
            tmp.library_path = fullPath;
            tmp.path = it.path().string();
            manifests.push_back(tmp);
        }
    }

    return manifests;
}

/**
 * @brief Converts a list of AppManifest objects into Game objects.
 *
 * Reads each manifest file, extracts information (name, install_dir, appid),
 * and stores it in a Game object.
 *
 * @param manifests Vector of AppManifest objects to parse.
 * @return Vector of Game objects representing installed games.
 */
std::vector<Game> SteamLibrary::getGames(const std::vector<AppManifest> &manifests) {
    std::vector<Game> games;

    for (const auto &manifest : manifests) {
        Game g;
        std::string content = getContent(manifest.path);

        g.library_path = manifest.library_path;
        g.manifest_path = manifest.path;

        std::string appidStr = parseManifest(content, "appid");
        if (appidStr == "228980") continue;
        if (!appidStr.empty()) g.appid = std::stoi(appidStr);

        g.install_dir = parseManifest(content, "installdir");
        g.name = parseManifest(content, "name");

        games.push_back(g);
    }

    return games;
}

void SteamLibrary::refresh(){
    std::string content = getContent(library_path);
    std::vector<std::string> escape_paths = getPaths(content, "path");
    std::vector<std::string> library_paths = unescapePaths(escape_paths);
    std::vector<AppManifest> manifests = getManifests(library_paths, R"(\steamapps)");
    std::vector<Game> games = getGames(manifests);

    m_cache.save(games);
}
std::vector<Game> SteamLibrary::loadGames(){
    return m_cache.load();
}
