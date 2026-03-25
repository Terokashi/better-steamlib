#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <windows.h>

#include "cli_parser.h"
#include "steam_parser.h"
#include "printer.h"
#include "cache.h"
#include "api_client.h"

/**
 * TODO:
 * Remove eager enrichment during rebuild
 * Add “does this command need genres?” logic
 * Implement lazy enrichment
 * Add per-app cache
 * Add multithreading
 * Add cache-dirty tracking
 * Add offline mode
 */

bool needGenres(const Command &cmd)
{
    if(cmd.group_key == GroupKey::Genre || cmd.sort_key == SortKey::Genre) return true;
    for (const auto &filter : cmd.filters){
        if (filter.key == FilterKey::Genre) return true;
    }
    return false;
}

/**
 * @file main.cpp
 * @brief Entry point for the Steam library CLI tool.
 *
 * This application loads the user's Steam libraries, builds a cache of installed games,
 * optionally enriches them with Steam metadata (e.g., genres), and displays the results
 * according to CLI options (sorting, grouping, and listing or debugging output).
 */

int main(int argc, char* argv[]) {
	if (argc > 1 && std::string(argv[1]) == "--help" | std::string(argv[i]) == "-h" {
    std::cout << R"(
Better Steam Library CLI Tool

Usage:
    better-steamlib.exe [OPTIONS]

Options:
    -l, --list           List all games
    -s, --sort <key>     Sort games by key
    -g, --group <key>    Group games by key
    -f, --filter <key> [values...]   Filter games
    -d, --descending     Descending sort
    -r, --refresh        Force refresh cache
	-h, --help           Show this help message
	debug                Output debug info
)" << std::endl;
    return 0;
}
    // Enable UTF-8 output in the Windows console
    SetConsoleOutputCP(CP_UTF8);

    // Parse command-line arguments into a Command object
    Command cmd = parseArgs(argc, argv);
    bool needsGenres = needGenres(cmd);

    // Paths and cache configuration
    const std::string std_library_path = R"(C:\Program Files (x86)\Steam\steamapps\libraryfolders.vdf)";
    const std::string apps_folder = R"(\steamapps)";
    const std::string cache_path = "cache/games.json";
    const std::string find_key = "path";

    std::vector<AppManifest> manifests;
    std::vector<Game> games;

    // Load Steam library folders
    std::string content = getContent(std_library_path);
    auto escape_paths = getPaths(content, find_key);
    auto library_paths = unescapePaths(escape_paths);


    // Load cached game data if available, otherwise rebuild
    if (!cmd.force_refresh && std::filesystem::exists(cache_path)) {
        std::cout << "[CACHE] Loaded from cache." << std::endl;
        games = loadCache(cache_path);
    } else {
        std::cout << "[CACHE] Rebuilding cache..." << std::endl;
        manifests = getManifests(library_paths, apps_folder);
        games = getGames(manifests);
        saveCache(games, cache_path);
    }

    if (needsGenres) {
        std::cout << "[ENRICH] Loading genres..." << std::endl;
        enrichGamesParallel(games);
    }

    // Handle CLI actions
    if (cmd.action == Action::List) {
        // Print all games according to the selected grouping and sorting
        printGames(games, cmd.group_key, cmd.sort_key, cmd.filters ,cmd.descending);
    }
    if (cmd.action == Action::Debug) {
        // Output debug information
        std::cout << "\n=== DEBUG ===\n";
        std::cout << "Games loaded: " << games.size() << "\n";
        std::cout << "[DEBUG] sort key = " << toString(cmd.sort_key) << "\n";
        std::cout << "[DEBUG] group key = " << toString(cmd.group_key) << "\n";
    }

    return 0;
}
