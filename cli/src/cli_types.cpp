#include <stdexcept>
#include "cli_types.h"
#include "game.h"

/**
 * @brief Parses a string into a SortKey enum.
 *
 * Converts a textual representation of a sort key into the corresponding
 * SortKey enum value.
 *
 * @param input The string to parse ("name", "install", "library", "appid").
 * @return The corresponding SortKey value.
 * @throws std::invalid_argument If the input does not match a valid key.
 */
SortKey cli::parseSortKey(const std::string &input)
{
    if (input == "name") return SortKey::Name;
    if (input == "install") return SortKey::InstallPath;
    if (input == "library") return SortKey::LibPath;
    if (input == "genre") return SortKey::Genre;
    if (input == "tag") return SortKey::Tag;

    return SortKey::None;
}

/**
 * @brief Parses a string into a GroupKey enum.
 *
 * Converts a textual representation of a group key into the corresponding
 * GroupKey enum value.
 *
 * @param input The string to parse ("name", "library", "install").
 * @return The corresponding GroupKey value.
 * @throws std::invalid_argument If the input does not match a valid key.
 */
GroupKey cli::parseGroupKey(const std::string &input)
{
    if (input == "name") return GroupKey::Name;
    if (input == "library") return GroupKey::LibPath;
    if (input == "install") return GroupKey::InstallPath;
    if (input == "genre") return GroupKey::Genre;
    if (input == "tag") return GroupKey::Tag;

    return GroupKey::None;
}

FilterKey cli::parseFilterKey(const std::string &input)
{
    if (input == "genre") return FilterKey::Genre;
    if (input == "name") return FilterKey::Name;
    if (input == "library") return FilterKey::LibPath;
    if (input == "install") return FilterKey::InstallPath;
    if (input == "tag") return FilterKey::Tag;

    return FilterKey::None;
}

/**
 * @brief Converts a SortKey enum to a human-readable string.
 *
 * @param key The SortKey value to convert.
 * @return A string representing the SortKey.
 * @throws std::invalid_argument If an invalid SortKey is provided.
 */
std::string cli::toString(SortKey key)
{
    switch(key) {
    case SortKey::Name:
        return "name";
    case SortKey::InstallPath:
        return "Install path";
    case SortKey::LibPath:
        return "Library path";
    case SortKey::Genre:
        return "Genre";
    case SortKey::Tag:
        return "Tag";
    case SortKey::None:
        return "None";
    }
    throw std::invalid_argument("Invalid sort key");
}

/**
 * @brief Converts a GroupKey enum to a human-readable string.
 *
 * @param key The GroupKey value to convert.
 * @return A string representing the GroupKey.
 * @throws std::invalid_argument If an invalid GroupKey is provided.
 */
std::string cli::toString(GroupKey key)
{
    switch(key) {
    case GroupKey::None:
        return "None";
    case GroupKey::LibPath:
        return "Library path";
    case GroupKey::InstallPath:
        return "Install path";
    case GroupKey::Genre:
        return "Genre";
    case GroupKey::Name:
        return "Name";
    case GroupKey::Tag:
        return "Tag";
    }
    throw std::invalid_argument("Invalid group key");
}

std::string cli::toString(FilterKey key)
{
    switch(key) {
    case FilterKey::None:
        return "None";
    case FilterKey::Genre:
        return "Genre";
    case FilterKey::InstallPath:
        return "Install Path";
    case FilterKey::LibPath:
        return "Library path";
    case FilterKey::Name:
        return "Name";
    case FilterKey::Tag:
        return "Tag";
    }
    throw std::invalid_argument("Invalid filter key");
}