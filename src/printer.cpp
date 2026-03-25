#include "printer.h"
#include <iostream>
#include <algorithm>

/**
 * @brief Groups games by a specified key.
 *
 * Supported keys: LibraryPath, InstallPath, Name.
 * If GroupKey::None is provided, all games are treated as a single group
 * under an empty string key.
 *
 * @param games Vector of Game objects to group.
 * @param key Key to group by.
 * @return std::unordered_map<std::string, std::vector<Game>>
 *         A map where each key is a group and the value is a vector of games in that group.
 */
std::unordered_map<std::string, std::vector<Game>>
groupGames(const std::vector<Game> &games, GroupKey key) {
    std::unordered_map<std::string, std::vector<Game>> grouped;

    for (const Game &g : games) {
        std::string groupKey;
        switch (key) {
        case GroupKey::LibraryPath:
            groupKey = g.library_path;
            break;
        case GroupKey::InstallPath:
            groupKey = g.install_dir;
            break;
        case GroupKey::Name:
            groupKey = g.name;
            break;
        case GroupKey::Genre:
            for (const std::string &genre : g.genres) {
                grouped[genre].push_back(g);
            }
            continue;
        case GroupKey::None:
            break;
        }
        grouped[groupKey].push_back(g);
    }

    return grouped;
}

// Helper for case-insensitive comparison
std::string toLower(const std::string &s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c){ return std::tolower(c); });
    return out;
}

// Combine multiple filters of the same type
std::unordered_map<FilterKey, std::vector<std::string>>
combineFilters(const std::vector<Filter> &filters) {
    std::unordered_map<FilterKey, std::vector<std::string>> combined;
    for (const auto &f : filters) {
        auto &vec = combined[f.key];
        vec.insert(vec.end(), f.val.begin(), f.val.end());
    }
    return combined;
}

// Filter games
std::vector<Game> filterGames(const std::vector<Game> &games, const std::vector<Filter> &filters) {
    std::vector<Game> filtered_games;
    auto combined_filters = combineFilters(filters);

    for (const Game &g : games) {
        bool keep_game = true;

        for (const auto &[key, values] : combined_filters) {
            bool found_match = false;

            switch (key) {
                case FilterKey::Name:
                    for (const auto &val : values) {
                        if (toLower(g.name).find(toLower(val)) != std::string::npos) {
                            found_match = true;
                            break;
                        }
                    }
                    break;

                case FilterKey::InstallPath:
                    for (const auto &val : values) {
                        if (g.install_dir.rfind(val, 0) == 0) { // starts-with
                            found_match = true;
                            break;
                        }
                    }
                    break;

                case FilterKey::LibraryPath:
                    for (const auto &val : values) {
                        if (g.library_path.rfind(val, 0) == 0) { // starts-with
                            found_match = true;
                            break;
                        }
                    }
                    break;

                case FilterKey::Genre:
                    for (const auto &val : values) {
                        // works whether g.genres is vector or unordered_set
                        if (std::find(g.genres.begin(), g.genres.end(), val) != g.genres.end()) {
                            found_match = true;
                            break;
                        }
                    }
                    break;

                case FilterKey::None:
                    found_match = true;
                    break;
            }

            if (!found_match) {  // AND logic across keys
                keep_game = false;
                break;
            }
        }

        if (keep_game) {
            filtered_games.push_back(g);
        }
    }

    return filtered_games;
}

std::string getSortValue(const Game &g) {
    std::vector<std::string> sorted_genres(g.genres.begin(), g.genres.end());
    std::sort(sorted_genres.begin(), sorted_genres.end());

    std::string genre_out;
    for (const std::string& genre : g.genres) {
        if (!genre_out.empty())
            genre_out += ", ";
        genre_out += genre;
    }
    return genre_out;
};

/**
 * @brief Sorts a vector of Game objects in-place.
 *
 * Supported SortKey values: Name, InstallPath, LibraryPath, AppID.
 * Sorting is lexicographical for strings, numeric for AppID.
 *
 * @param games Vector of Game objects to sort.
 * @param key Sorting key.
 * @param descending True to sort in descending order, false for ascending.
 */
void sortGames(std::vector<Game> &games, SortKey key, bool descending) {
    std::sort(games.begin(), games.end(),
              [&key, descending](const Game &a, const Game &b) {
                  bool result = false;
                  switch (key) {
                  case SortKey::Name:
                      result = a.name < b.name;
                      break;
                  case SortKey::InstallPath:
                      result = a.install_dir < b.install_dir;
                      break;
                  case SortKey::LibraryPath:
                      result = a.library_path < b.library_path;
                      break;
                  case SortKey::AppID:
                      result = a.appid < b.appid;
                      break;
                  case SortKey::Genre: {
                      std::string a_key, b_key;
                      a_key = getSortValue(a);
                      b_key = getSortValue(b);
                      result = a_key < b_key;
                      break;
                  }
                  case SortKey::None:
                      break;
                  }
                  return descending ? !result : result;
              });
}

/**
 * @brief Prints a vector of Game objects to the console, optionally grouped and sorted.
 *
 * If grouping is enabled (GroupKey != None), games are first grouped and then
 * each group is sorted individually. Each game displays its name and the value
 * corresponding to the sort key.
 *
 * @param games Vector of Game objects to print.
 * @param group_key Optional key to group by. Use GroupKey::None for no grouping.
 * @param sort_key Key to sort by within each group.
 * @param descending True for descending sort, false for ascending.
 */
void printGames(
    std::vector<Game> &games,
    GroupKey group_key,
    SortKey sort_key,
    std::vector<Filter> Filters,
    bool descending
    ) {
    if (games.empty()) {
        std::cout << "[No games to display]" << std::endl;
        return;
    }

    std::vector<Game> filtered_games = filterGames(games, Filters);
    if (filtered_games.empty())
    {
        std::cout << "[No games]";
        return;
    }

    // Lambda to get string representation of the sort value
    auto getSortValue = [sort_key](const Game &g) -> std::string {
        switch (sort_key) {
        case SortKey::Name: return g.name;
        case SortKey::InstallPath: return g.install_dir;
        case SortKey::LibraryPath: return g.library_path;
        case SortKey::AppID: return std::to_string(g.appid);
        case SortKey::Genre: {
            std::string genre_out;
            for (const std::string& genre : g.genres) {
                if (!genre_out.empty())
                    genre_out += ", ";
                genre_out += genre;
            }
            return genre_out;
        }
        case SortKey::None: break;
        }
        return g.name; // fallback
    };

    // If no grouping, just sort and print
    if (group_key == GroupKey::None) {
        std::vector<Game> sorted_games = filtered_games;
        sortGames(sorted_games, sort_key, descending);
        for (const Game &g : sorted_games)
            std::cout << "- " << g.name << " [" << getSortValue(g) << "]" << std::endl;
        return;
    }

    // Grouped printing
    auto grouped = groupGames(filtered_games, group_key);
    std::vector<std::string> keys;
    for (const auto &[k, _] : grouped) keys.push_back(k);

    // Sort the group keys
    std::sort(keys.begin(), keys.end(), [descending](const std::string &a, const std::string &b) {
        return descending ? a > b : a < b;
    });

    // Print each group
    for (const std::string &k : keys) {
        std::vector<Game> sortedGroup = grouped[k];
        sortGames(sortedGroup, sort_key, descending);

        std::cout << "[" << k << "]" << std::endl;
        for (const Game &g : sortedGroup)
            std::cout << "- " << g.name << " [" << getSortValue(g) << "]" << std::endl;
        std::cout << std::endl;
    }
}