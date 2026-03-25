# better-steamlib
Simply a better steam library: More sorting, grouping and filtering.

### Build with CMake (Windows + MinGW)

```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### Description:
A command-line tool to list, group, sort, and filter your installed Steam games. Optionally enriches games with genres from the Steam Store API.

### Usage
```better-steamlib.exe [OPTIONS]```

| Option                           | Description                                                                                                                                                     |
| -------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `-l, --list`                     | List all games according to sorting, grouping, and filters.                                                                                                     |
| `-s, --sort <key>`               | Sort games by a key. Available keys: `name`, `genre`, `appid`, `path`, `last_played`.                                                                           |
| `-g, --group <key>`              | Group games by a key. Available keys: `name`, `genre`, `path`.                                                                                                  |
| `-f, --filter <key> [values...]` | Filter games by key and one or more values. Examples: `--filter genre Action Adventure`, `-f path "C:\SteamLibrary"`. Available keys: `genre`, `path`, `appid`. |
| `-d, --descending`               | Sort in descending order (default is ascending).                                                                                                                |
| `-r, --refresh`                  | Force rebuild of cached game data from library folders.                                                                                                         |
| `debug`                          | Output debug information about loaded games, cache status, and CLI options.                                                                                     |
| `--help`                         | Show this help message.                                                                                                                                         |

Examples
```
# List all games grouped by genre and sorted by name
better-steamlib.exe -l -g genre -s name

# List games filtered by genre Action and Adventure
better-steamlib.exe -l -f genre Action Adventure

# Rebuild cache and list all games
better-steamlib.exe -r -l

# Output debug information
better-steamlib.exe debug
```
