#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "game.h"
#include "cli_types.h"

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
groupGames(const std::vector<Game> &games, GroupKey key);

std::vector<Game> filterGames(const std::vector<Game> &games, const std::vector<Filter> &Filters);

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
void sortGames(std::vector<Game> &games, SortKey key, bool descending = false);

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
    GroupKey group_key,
    SortKey sort_key,
    std::vector<Filter> Filters,
    bool descending = false
    );