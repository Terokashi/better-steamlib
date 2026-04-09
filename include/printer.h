#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "game.h"
#include "cli_types.h"

struct Row {
    std::string name;   // first column
    std::string genres; // second column
    // std::string size;   // third column (not implemented)
    std::string path;   // fourth column
    std::string sort_value;
};

struct Widths {
    int max_name_width = 0;
    int max_genre_width = 0;
    int max_path_width = 0;
    int max_sort_width = 0;
};

std::vector<Game> filterGames(const std::vector<Game> &games, const std::vector<cli::Filter> &Filters);

/**
 * @brief Groups games based on a specified key.
 *
 * This function organizes a list of Game objects into groups,
 * where each group is identified by a string key derived from
 * the specified GroupKey.
 *
 * @param games The list of games to group.
 * @param key The grouping criterion.
 * @return An unordered_map where each key represents a group name
 *         and the value is a vector of Game objects in that group.
 */
std::unordered_map<std::string, std::vector<Game>>
groupGames(const std::vector<Game> &games, cli::GroupKey key);

/**
 * @brief Sorts a list of games in-place.
 *
 * This function sorts the provided vector of Game objects according
 * to the specified SortKey and order.
 *
 * @param games The list of games to sort.
 * @param key The sorting criterion.
 * @param descending If true, sorts in descending order; otherwise ascending.
 */
void sortGames(std::vector<Game> &games, cli::SortKey key, bool descending = false);

std::vector<Row> buildRows(const std::vector<Game> &games, cli::SortKey &sort_key);

Widths computeColumnWidths(const std::vector<Row> &rows);

void printGroupHeader(const std::string &group_name, std::size_t game_count);

std::string shorten(const std::string toShorten, int width);

void printRow(const Row &row, Widths &width);

void printRows(std::vector<Row> &formatted_str, Widths &widths);

std::string formatGenres(std::unordered_set<std::string> &genres);



/**
 * @brief Prints a list of games with optional grouping and sorting.
 *
 * This function handles sorting and grouping of games before
 * outputting them, typically to the console.
 *
 * @param games The list of games to print.
 * @param group_key The grouping criterion.
 * @param sort_key The sorting criterion.
 * @param descending If true, sorts in descending order; otherwise ascending.
 */
void printGames(
    std::vector<Game> &games,
    cli::GroupKey group_key,
    cli::SortKey sort_key,
    std::vector<cli::Filter> Filters,
    bool descending = false
    );