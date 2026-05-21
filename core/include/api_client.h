#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <mutex>

#include "game.h"

class ApiClient {
public:
    ApiClient(const std::string &baseUrl,
              const std::string &authToken = "",
              const std::string &cachePath = "cache/apps/")
        : base_url(baseUrl), auth_token(authToken), cache_path(cachePath){}

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
    std::unordered_set<std::string> fetchGenres(std::string appid);

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

private:
    std::string base_url;
    std::string auth_token;
    std::string cache_path;
    std::unordered_map<std::string, std::unordered_set<std::string>> genre_cache;

    std::mutex cout_mutex;

    std::string httpGet(const std::string &url);
    void saveToCache(std::string appid, const std::unordered_set<std::string> &genres);
    std::unordered_set<std::string> loadFromCache(std::string appid);

    void logger(const std::string &log);
};



