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
 *
 * IMPORTANT:
 * fix cli_parser logic to include custom tags
 */

void enableANSI()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

void initConsole()
{
#ifdef _WIN32
    enableANSI();
    // Enable UTF-8 output in the Windows console
    SetConsoleOutputCP(CP_UTF8);
#endif
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
    if (argc > 1 && std::string(argv[1]) == "--help" | std::string(argv[1]) == "-h") {
        std::cout << R"(
Better Steam Library CLI Tool

Usage:
    better-steamlib.exe [OPTIONS]

Options:
    -l, --list          List all games
    -s, --sort <key>    Sort games by key
    -g, --group <key>   Group games by key
    -f, --filter <key>  [values...]   Filter
    -t, --tag <appid>    [values...]
    -d, --descending    Descending sort
    -r, --refresh       Force refresh cache
    debug               Output debug info
    --help              Show this help message
)" << std::endl;
        return 0;
    }
    initConsole();

    // Parse command-line arguments into a Command object
    Command cmd = parseArgs(argc, argv);
    ApiClient client("https://store.steampowered.com/api/appdetails?appids=");

    // Paths and cache configuration
    const std::string std_library_path = R"(C:\Program Files (x86)\Steam\steamapps\libraryfolders.vdf)";
    const std::string apps_folder = R"(\steamapps)";
    const std::string cache_path = "cache/games.json";
    const std::string find_key = "path";

    std::vector<AppManifest> manifests;
    SteamLibrary steamlib;

    GameCache cache("cache/games.json");

    steamlib.refresh(); // build cache if needed
    std::vector<Game> games = steamlib.loadGames();
    client.enrichGamesParallel(games);

    // Lazy enrichment
    bool needEnrichment = cmd.force_refresh;
    for (const auto &g : games)
        if (g.genres.empty()) { needEnrichment = true; break; }

    if (needEnrichment)
        client.enrichGamesParallel(games);

    if(!cmd.tags.empty()) {
        for (const auto &t : cmd.tags)
        {
            bool found = false;
            std::cout << "[TAGGING] appid: " << std::stoi(t.key) << std::endl;
            for(Game &g : games)
            {
                std::cout << "[TAGGING] game: " << g.appid << " " << g.name << std::endl;
                if (g.appid == std::stoi(t.key)) {
                    found = true;
                    std::copy(t.val.begin(), t.val.end(),
                              std::inserter(g.tags, g.tags.end()));
                }
            }

            if (!found) {
                std::cout << "[WARNING] No game with appid " << t.key << "\n";
            }
        }
    }

    // Handle CLI actions
    if (cmd.action == cli::Action::List) {
        // Print all games according to the selected grouping and sorting
        printGames(games, cmd.group_key, cmd.sort_key, cmd.filters ,cmd.descending);
    }

    if (cmd.action == cli::Action::Debug) {
        // Output debug information
        std::cout << "\n=== DEBUG ===\n";
        std::cout << "Games loaded: " << games.size() << "\n";
        std::cout << "[DEBUG] sort key = " << cli::toString(cmd.sort_key) << "\n";
        std::cout << "[DEBUG] group key = " << cli::toString(cmd.group_key) << "\n";
    }
    return 0;
}