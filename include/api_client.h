#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include "game.h"

/**
 * @brief Fetches genre information for a given game.
 *
 * Queries an external source (e.g., API or local database) to retrieve
 * the list of genres associated with the specified application ID.
 *
 * @param appid The unique application ID of the game.
 * @return A vector of genre names. Returns an empty vector if no genres are found
 *         or if the fetch operation fails.
 */
std::unordered_set<std::string> fetchGenres(int appid);

/**
 * @brief Enriches a single Game object with genre information.
 *
 * Uses the game's application ID to fetch genre data and updates
 * the provided Game instance accordingly.
 *
 * @param game Reference to the Game object to enrich.
 */
void enrichGameWithGenres(Game &game);

/**
 * @brief Enriches multiple Game objects with genre information.
 *
 * Iterates over a list of games and populates each one with its
 * corresponding genre data.
 *
 * @param games Reference to a vector of Game objects to enrich.
 */
void enrichGamesParallel(std::vector<Game> &games, size_t max_concurrent = 10);