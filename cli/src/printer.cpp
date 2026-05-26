#include "printer.h"
#include "style.h"

#include <iostream>
#include <iomanip>

#include <algorithm>



// Helper for case-insensitive comparison
std::string toLower(const std::string &s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c){ return std::tolower(c); });
    return out;
}

// Combine multiple filters of the same type
std::unordered_map<FilterKey, std::vector<std::string>>
combineFilters(const std::vector<cli::Filter> &filters) {
    std::unordered_map<FilterKey, std::vector<std::string>> combined;
    for (const auto &f : filters) {
        auto &vec = combined[f.key];
        vec.insert(vec.end(), f.val.begin(), f.val.end());
    }
    return combined;
}


// Filter games
std::vector<Game> filterGames(const std::vector<Game> &games, const std::vector<cli::Filter> &filters) {
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
            case FilterKey::LibPath:
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
            case FilterKey::Tag:
                for (const auto &val : values) {
                    if (std::find(g.tags.begin(), g.tags.end(), val) != g.tags.end()) {
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
        case GroupKey::LibPath:
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
        case GroupKey::Tag:
            for (const std::string &tag : g.tags) {
                grouped[tag].push_back(g);
            }
        case GroupKey::None:
            break;
        }
        grouped[groupKey].push_back(g);
    }

    return grouped;
}


std::string getSortValue(const std::unordered_set<std::string> &list) {
    std::vector<std::string> sorted_list(list.begin(), list.end());
    std::sort(sorted_list.begin(), sorted_list.end());

    std::string list_out;
    for (const std::string &l : list) {
        if (!list_out.empty())
            list_out += ", ";
        list_out += l;
    }
    return list_out;
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
                  case SortKey::LibPath:
                      result = a.library_path < b.library_path;
                      break;
                  case SortKey::Genre: {
                      std::string a_key, b_key;
                      a_key = getSortValue(a.genres);
                      b_key = getSortValue(b.genres);
                      result = a_key < b_key;
                      break;
                  }
                  case SortKey::Tag: {
                      std::string a_key, b_key;
                      a_key = getSortValue(a.tags);
                      b_key = getSortValue(b.tags);
                      result = a_key < b_key;
                      break;
                  }

                  case SortKey::None:
                      break;
                  }
                  return descending ? !result : result;
              });
}

std::string formatGenres(const std::unordered_set<std::string> &genres) {
    std::vector<std::string> genres_to_sort(genres.begin(), genres.end());
    std::sort(genres_to_sort.begin(), genres_to_sort.end());
    std::string formatted_genres;
    for (size_t i = 0; i < genres_to_sort.size(); ++i) {
        formatted_genres += genres_to_sort[i];
        if (i < genres_to_sort.size() - 1)
            formatted_genres += ", ";
    }
    return formatted_genres;
}

std::string formatTags(const std::unordered_set<std::string> &tags) {
    std::vector<std::string> tags_to_sort(tags.begin(), tags.end());
    std::sort(tags_to_sort.begin(), tags_to_sort.end());
    std::string formatted_tags;
    for(size_t i = 0; i < tags_to_sort.size(); i++) {
        formatted_tags += tags_to_sort[i];
        if (i < tags_to_sort.size() - 1)
            formatted_tags += ", ";
    }
    return formatted_tags;
}

std::string sanitize(std::string to_sanitize)
{
    std::string sanitized;
    for (const char &c : to_sanitize)
    {
        if (!(c < 32) && !(c > 124)) sanitized += c;
    }
    return sanitized;
}

std::vector<Row> buildRows(const std::vector<Game> &games, SortKey &sort_key) {
    std::vector<Row> rows;
    // Go through each game
    for (const Game &g : games)
    {
        Row tmp_row;
        tmp_row.genres = sanitize(formatGenres(g.genres));
        tmp_row.name = sanitize(g.name);
        tmp_row.path = sanitize(g.install_dir);

        switch(sort_key) {
        case SortKey::Genre:
            tmp_row.sort_value = sanitize(formatGenres(g.genres));
            break;
        case SortKey::InstallPath:
            tmp_row.sort_value = sanitize(g.install_dir);
            break;
        case SortKey::LibPath:
            tmp_row.sort_value = sanitize(g.library_path);
            break;
        case SortKey::Name:
            tmp_row.sort_value = sanitize(g.name);
            break;
        case SortKey::Tag:
            tmp_row.sort_value = sanitize(formatTags(g.tags));
            break;
        case SortKey::None:
            tmp_row.sort_value = "";
            break;
        }

        rows.push_back(tmp_row);
    }

    return rows;
}

Widths computeColumnWidths(const std::vector<Row> &rows) {
    Widths max_widths;

    // get real Max
    for(const Row &r : rows)
    {
        if(r.genres.length() > max_widths.max_genre_width)
            max_widths.max_genre_width = r.genres.length();

        if(r.name.length() > max_widths.max_name_width)
            max_widths.max_name_width = r.name.length();

        if(r.path.length() > max_widths.max_path_width)
            max_widths.max_path_width = r.path.length();
        if(r.sort_value.length() > max_widths.max_sort_width)
            max_widths.max_sort_width = r.sort_value.length();
    }

    // check if real Max too big
    if(max_widths.max_genre_width > 50) max_widths.max_genre_width = 50;
    if(max_widths.max_name_width > 40) max_widths.max_name_width = 40;
    if(max_widths.max_path_width > 60) max_widths.max_path_width = 60;
    if(max_widths.max_sort_width > 60) max_widths.max_sort_width = 60;

    // check if real Max too small
    if(max_widths.max_genre_width < 13) max_widths.max_genre_width = 13;
    if(max_widths.max_name_width < 11) max_widths.max_name_width = 11;
    if(max_widths.max_path_width < 14) max_widths.max_path_width = 14;
    if(max_widths.max_sort_width < 12) max_widths.max_sort_width = 12;

    return max_widths;
}

std::string shorten(const std::string toShorten, int width)
{
    std::string shortened;

    if(toShorten.length() > width)
    {
        shortened = toShorten.substr(0, width - 3);
        shortened += "...";
    }
    else shortened = toShorten;

    return shortened;
}

void printSeparator(Widths &width) {
    std::cout << "+"
              << std::string(width.max_name_width + 2, '-') << "+"
              << std::string(width.max_genre_width + 2, '-') << "+"
              << std::string(width.max_path_width + 2, '-') << "+"
              << std::string(width.max_sort_width + 2, '-') << "+"
              << std::endl;
}

std::string padStyle(const std::string &txt, int width, const Style &style)
{
    std::string padded_styled_str = txt;

    while(padded_styled_str.length() < width)
    {
        padded_styled_str += " ";
    }

    padded_styled_str = applyStyle(padded_styled_str, style);
    return padded_styled_str;
}

void printColumnHeader(Widths &width) {
    std::cout << std::setfill(' ')
              << std::left
              << "| "
              << padStyle("Game Name", width.max_name_width, nameStyle) << " | "
              << padStyle("Game Genres", width.max_genre_width, genreStyle) << " | "
              << padStyle("Game Install", width.max_path_width, pathStyle) << " | "
              << padStyle("Sort Value", width.max_sort_width, sortStyle) << " |"
              << std::endl;
}

void printRow(const Row &row, Widths &width) {
    std::string name = shorten(row.name, width.max_name_width);
    std::string genre = shorten(row.genres, width.max_genre_width);
    std::string path = shorten(row.path, width.max_path_width);
    std::string sort = shorten(row.sort_value, width.max_sort_width);

    std::cout << "| "
              << padStyle(name, width.max_name_width, nameStyle) << " | "
              << padStyle(genre, width.max_genre_width, genreStyle) << " | "
              << padStyle(path, width.max_path_width, pathStyle) << " | "
              << padStyle(sort, width.max_sort_width, sortStyle) << " |"
              << std::endl;

}

void printRows(std::vector<Row> &rows, Widths &widths) {
    printColumnHeader(widths);
    printSeparator(widths);
    for (const Row &r : rows){
        printRow(r, widths);
    }
    printSeparator(widths);
    std::cout << std::endl;
}

void printGroupHeader(const std::string &groupName, const GroupKey &group_key , std::size_t game_count) {
    std::string out;
    int len;
    if(group_key == GroupKey::LibPath)
    {
        out += "(";                 // add opening parenthesis
        out += groupName[0];        // add first character of groupName
        out += ":\\) Game count: "; // add rest of the text
        out += std::to_string(game_count);
        len = out.length();
        std::cout << padStyle(out, len, headerStyle) << std::endl;
        std::cout << std::setfill('=')
                  << std::setw(len) << "=" << std::endl;
        return;
    }
    out += "(" + groupName + ") Game count: " + std::to_string(game_count);
    len = out.length();
    std::cout << padStyle(out, len, headerStyle) << std::endl;
    std::cout << std::setfill('=')
              << std::setw(len) << "=" << std::endl;

    return;
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
    std::vector<cli::Filter> Filters,
    bool descending
    ) {
    if (games.empty()) {
        std::cout << "[No games to display]" << std::endl;
        return;
    }

    std::vector<Game> filtered_games = filterGames(games, Filters);
    std::vector<Row> rows = buildRows(filtered_games, sort_key);
    Widths width = computeColumnWidths(rows);
    if (filtered_games.empty())
    {
        std::cout << "[No games for filter]";
        return;
    }

    // If no grouping, just sort and print
    if (group_key == GroupKey::None) {
        std::vector<Game> sorted_games = filtered_games;
        sortGames(sorted_games, sort_key, descending);

        rows = buildRows(sorted_games, sort_key);

        printRows(rows, width);
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

        printGroupHeader(k, group_key, sortedGroup.size());
        rows = buildRows(sortedGroup, sort_key);
        printRows(rows, width);
    }
    return;
}