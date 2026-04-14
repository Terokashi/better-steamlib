#include "cli_types.h"
#include <iostream>

/**
 * @brief Parses a string into a cli::SortKey enum.
 *
 * Converts a textual representation of a sort key into the corresponding
 * cli::SortKey enum value.
 *
 * @param input The string to parse ("name", "install", "library", "appid").
 * @return The corresponding cli::SortKey value.
 * @throws std::invalid_argument If the input does not match a valid key.
 */
cli::SortKey cli::parseSortKey(const std::string &input)
{
    if (input == "name") return cli::SortKey::Name;
    if (input == "install") return cli::SortKey::InstallPath;
    if (input == "library") return cli::SortKey::LibraryPath;
    if (input == "appid") return cli::SortKey::AppID;
    if (input == "genre") return cli::SortKey::Genre;
    if (input == "tag") return cli::SortKey::Tag;
    std::cout << "Using no sorting!\n";

    return cli::SortKey::None;
}

/**
 * @brief Parses a string into a cli::GroupKey enum.
 *
 * Converts a textual representation of a group key into the corresponding
 * cli::GroupKey enum value.
 *
 * @param input The string to parse ("name", "library", "install").
 * @return The corresponding cli::GroupKey value.
 * @throws std::invalid_argument If the input does not match a valid key.
 */
cli::GroupKey cli::parseGroupKey(const std::string &input)
{
    if (input == "name") return cli::GroupKey::Name;
    if (input == "library") return cli::GroupKey::LibraryPath;
    if (input == "install") return cli::GroupKey::InstallPath;
    if (input == "genre") return cli::GroupKey::Genre;
    if (input == "tag") return cli::GroupKey::Tag;
    std::cout << "Using no grouping!\n";

    return cli::GroupKey::None;
}

cli::FilterKey cli::parseFilterKey(const std::string &input)
{
    if (input == "genre") return cli::FilterKey::Genre;
    if (input == "name") return cli::FilterKey::Name;
    if (input == "library") return cli::FilterKey::LibraryPath;
    if (input == "install") return cli::FilterKey::InstallPath;
    if (input == "tag") return cli::FilterKey::Tag;
    std::cout << "Using no Filter!\n";

    return cli::FilterKey::None;
}

/**
 * @brief Converts a cli::SortKey enum to a human-readable string.
 *
 * @param key The cli::SortKey value to convert.
 * @return A string representing the cli::SortKey.
 * @throws std::invalid_argument If an invalid cli::SortKey is provided.
 */
std::string cli::toString(cli::SortKey key)
{
    switch(key) {
    case cli::SortKey::Name:
        return "name";
    case cli::SortKey::InstallPath:
        return "Install path";
    case cli::SortKey::LibraryPath:
        return "Library path";
    case cli::SortKey::AppID:
        return "AppID";
    case cli::SortKey::Genre:
        return "Genre";
    case cli::SortKey::Tag:
        return "Tag";
    case cli::SortKey::None:
        return "None";
    }
    throw std::invalid_argument("Invalid sort key");
}

/**
 * @brief Converts a cli::GroupKey enum to a human-readable string.
 *
 * @param key The cli::GroupKey value to convert.
 * @return A string representing the cli::GroupKey.
 * @throws std::invalid_argument If an invalid cli::GroupKey is provided.
 */
std::string cli::toString(cli::GroupKey key)
{
    switch(key) {
    case cli::GroupKey::None:
        return "None";
    case cli::GroupKey::LibraryPath:
        return "Library path";
    case cli::GroupKey::InstallPath:
        return "Install path";
    case cli::GroupKey::Genre:
        return "Genre";
    case cli::GroupKey::Name:
        return "Name";
    case cli::GroupKey::Tag:
        return "Tag";
    }
    throw std::invalid_argument("Invalid group key");
}

std::string cli::toString(cli::FilterKey key)
{
    switch(key) {
    case cli::FilterKey::None:
        return "None";
    case cli::FilterKey::Genre:
        return "Genre";
    case cli::FilterKey::InstallPath:
        return "Install Path";
    case cli::FilterKey::LibraryPath:
        return "Library path";
    case cli::FilterKey::Name:
        return "Name";
    case cli::FilterKey::Tag:
        return "Tag";
    }
    throw std::invalid_argument("Invalid filter key");
}