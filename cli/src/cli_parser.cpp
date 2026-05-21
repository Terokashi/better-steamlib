#include "cli_parser.h"

/**
 * @brief Parses command-line arguments into a Command structure.
 *
 * This function interprets CLI arguments (argc/argv) and fills
 * a Command object with the corresponding action, sorting, grouping,
 * and modifier flags. Recognized arguments include:
 *
 * - `--list` or `-l` : Sets action to `Action::List`.
 * - `--sort` or `-s` : Sets the sorting key (requires a value).
 * - `--group` or `-g` : Sets the grouping key (requires a value).
 * - `--descending` or `-d` : Enables descending sort.
 * - `--refresh` or `-r` : Forces a refresh of cached data.
 * - `debug` : Sets action to `Action::Debug`.
 *
 * @param argc Number of command-line arguments (from main()).
 * @param argv Array of argument strings (from main()).
 * @return A populated Command structure reflecting the parsed arguments.
 */
Command parseArgs(int argc, char* argv[]) {
    Command cmd;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        auto hasValue = [&](int idx) {
            return (idx + 1 < argc) && argv[idx + 1][0] != '-';
        };

        if (arg == "--list" || arg == "-l") {
            cmd.action = cli::Action::List;
        }
        else if (arg == "--sort" || arg == "-s") {
            if (hasValue(i)) cmd.sort_key = cli::parseSortKey(argv[++i]);
        }
        else if (arg == "--group" || arg == "-g") {
            if (hasValue(i)) cmd.group_key = cli::parseGroupKey(argv[++i]);
        }
        else if (arg == "--filter" || arg == "-f") {
            if (!hasValue(i)) continue;

            // Step 1: filter key
            cli::Filter f;
            i++;
            f.key = cli::parseFilterKey(argv[i]);

            // Step 2: filter values
            while (i + 1 < argc && argv[i + 1][0] != '-') {
                i++;
                f.val.push_back(argv[i]);
            }

            cmd.filters.push_back(f);
        }
        else if (arg == "--tag" || arg == "-t") {
            if (!hasValue(i)) continue;
            cli::Tag t;
            i++;
            t.key = argv[i];

            while (i + 1 < argc && argv[i + 1][0] != '-') {
                i++;
                t.val.push_back(argv[i]);
            }
            cmd.tags.push_back(t);
        }
        else if (arg == "--descending" || arg == "-d") {
            cmd.descending = true;
        }
        else if (arg == "--refresh" || arg == "-r") {
            cmd.force_refresh = true;
        }
        else if (arg == "debug") {
            cmd.action = cli::Action::Debug;
        }
    }

    return cmd;
}