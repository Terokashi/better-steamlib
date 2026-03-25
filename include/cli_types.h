#pragma once
#include <string>
#include <vector>

/**
 * @brief Keys used to sort Game entries.
 */
enum class SortKey {
    None,           /**< No sorting */
    Name,           /**< Sort by game name */
    InstallPath,    /**< Sort by installation path */
    LibraryPath,    /**< Sort by library path */
    AppID,          /**< Sort by application ID */
    Genre           /**< Sort by Genre */
};

/**
 * @brief Keys used to group Game entries.
 */
enum class GroupKey {
    None,           /**< No grouping */
    LibraryPath,    /**< Group by library path */
    InstallPath,    /**< Group by installation path */
    Name,           /**< Group by game name */
    Genre           /**< Group by Genre */
};

enum class FilterKey {
    None,           /**< No Filter */
    Genre,          /**< Filter by Genre */
    Name,           /**< Filter by game name */
    LibraryPath,    /**< Filter by library path */
    InstallPath     /**< Filter by Install path */
};

struct Filter {
    FilterKey key;
    std::vector<std::string> val;
};

/**
 * @brief Available actions for the CLI tool.
 */
enum class Action {
    List,   /**< List games */
    Debug   /**< Run in debug mode */
};

/**
 * @brief Parses a string into a SortKey.
 *
 * Converts user input (e.g., from CLI arguments) into the corresponding
 * SortKey enum value.
 *
 * @param input The input string representing the sort key.
 * @return The corresponding SortKey value.
 *
 * @throws std::invalid_argument If the input does not match a valid SortKey.
 */
SortKey parseSortKey(const std::string &input);

/**
 * @brief Parses a string into a GroupKey.
 *
 * Converts user input (e.g., from CLI arguments) into the corresponding
 * GroupKey enum value.
 *
 * @param input The input string representing the group key.
 * @return The corresponding GroupKey value.
 *
 * @throws std::invalid_argument If the input does not match a valid GroupKey.
 */
GroupKey parseGroupKey(const std::string &input);

/**
 * @brief Parses a string into a FilterKey.
 *
 * Converts user input (e.g., from CLI arguments) into the corresponding
 * SortKey enum value.
 *
 * @param input The input string representing the filter key.
 * @return the corresponding GroupKey Value
 *
 * @throws std::invalid_argument If the input does not match a valid FilterKey.
 */
FilterKey parseFilterKey(const std::string &input);

/**
 * @brief Converts a SortKey to its string representation.
 *
 * @param key The SortKey to convert.
 * @return A string representation of the SortKey.
 */
std::string toString(SortKey key);

/**
 * @brief Converts a GroupKey to its string representation.
 *
 * @param key The GroupKey to convert.
 * @return A string representation of the GroupKey.
 */
std::string toString(GroupKey key);

std::string toString(FilterKey key);