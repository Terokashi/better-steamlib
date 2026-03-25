#pragma once

#include <vector>
#include <string>
#include "game.h"

/**
 * @brief Loads a list of Game objects from a cache file.
 *
 * This function reads serialized Game data from the specified file path
 * and reconstructs a vector of Game objects.
 *
 * @param path The file system path to the cache file.
 * @return A std::vector<Game> containing all loaded Game objects.
 *
 * @throws std::runtime_error If the file cannot be opened or parsing fails.
 */
std::vector<Game> loadCache(const std::string& path);

/**
 * @brief Saves a list of Game objects to a cache file.
 *
 * This function serializes the provided vector of Game objects and writes
 * them to the specified file path.
 *
 * @param games The vector of Game objects to save.
 * @param path The file system path where the cache file will be written.
 *
 * @throws std::runtime_error If the file cannot be written.
 */
void saveCache(const std::vector<Game>& games, const std::string& path);