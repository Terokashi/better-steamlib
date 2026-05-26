#pragma once
#include "cli_types.h"
#include <vector>

/**
 * @brief Represents a parsed command from CLI arguments.
 *
 * This structure stores all options and flags derived from the
 * command-line input, including the selected action, sorting
 * and grouping preferences, and additional modifiers.
 */
struct Command {
    /**
     * @brief The action to perform.
     *
     * Defaults to Action::List if no specific action is provided.
     */
    cli::Action action = cli::Action::List;

    /**
     * @brief Key used for sorting results.
     *
     * Defaults to SortKey::Name.
     */
    SortKey sort_key = SortKey::Name;

    /**
     * @brief Key used for grouping results.
     *
     * Defaults to GroupKey::LibraryPath.
     */
    GroupKey group_key = GroupKey::LibPath;

    std::vector<cli::Filter> filters;
    std::vector<cli::Tag> tags;

    /**
     * @brief Whether sorting should be in descending order.
     *
     * Defaults to false (ascending order).
     */
    bool descending = false;

    /**
     * @brief Forces refresh of cached or stored data.
     *
     * When true, cached data may be ignored and reloaded.
     */
    bool force_refresh = false;
};

/**
 * @brief Parses command-line arguments into a Command structure.
 *
 * This function interprets the provided argc/argv parameters
 * and converts them into a structured Command object that
 * can be used by the application.
 *
 * @param argc Argument count from main().
 * @param argv Argument vector from main().
 * @return A populated Command structure representing the parsed input.
 *
 * @throws std::invalid_argument If invalid or conflicting arguments are provided.
 */
Command parseArgs(int argc, char* argv[]);