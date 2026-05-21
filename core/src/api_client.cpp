#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <future>

#include <iostream>
#include <sstream>

#include "api_client.h"


using json = nlohmann::json;

void to_json(json& j, const std::unordered_set<std::string> &genres)
{
    j = json{
        {"genres", genres}
    };
}
void from_json(const json &j, std::unordered_set<std::string> &genres)
{
    j.at("genres").get_to(genres);
}

/**
 * @brief Callback function for libcurl to write response data.
 *
 * This function appends received data to the provided std::string.
 *
 * @param contents Pointer to the received data.
 * @param size Size of each data element (usually 1).
 * @param nmemb Number of elements.
 * @param output Pointer to the std::string to append data to.
 * @return The number of bytes processed.
 */
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    output->append((char*)contents, size * nmemb);
    return size * nmemb;
}

/**
 * @brief Fetches the list of genres for a given Steam app.
 *
 * Queries the Steam Store API for the app's details and extracts
 * genre descriptions.
 *
 * @param appid The Steam application ID.
 * @return A vector of genre names. Empty if none are found or request fails.
 */
std::unordered_set<std::string> ApiClient::fetchGenres(std::string appid)
{
    std::string app_path = cache_path + appid + ".json";
    std::filesystem::create_directories(std::filesystem::path(app_path).parent_path());
    std::ostringstream oss;
    std::unordered_set<std::string> genres;

    // check if game already exists in cache
    if (std::filesystem::exists(app_path))
    {
        logger("[LOAD CACHE] " + appid);
        return loadFromCache(appid);
    }
    std::string url = base_url + appid + "&l=en&cc=US";

    logger("[API FETCH] " + appid);

    // Call Steam API
    std::string response = httpGet(url);
    if (response.empty()) {
        {
            logger("[API FETCH FAIL] " + appid);
        }
        return genres;
    }

    // Parse Steam response
    try {
        json j_response = json::parse(response);

        if (j_response.contains(appid) &&
            j_response[appid]["success"] == true &&
            j_response[appid]["data"].contains("genres"))
        {
            for (const auto &g : j_response[appid]["data"]["genres"])
            {
                genres.insert(g["description"]);
            }
        }
        else {
            {
                std::cout << j_response.dump(4);
                logger("[CACHE WRITE EMPTY] " + appid);
            }
        }
    } catch (const std::exception &e) {
        {
            logger("Something went wrong! " + std::string(e.what()));
        }
    }

    saveToCache(appid, genres);
    return genres;
}

/**
 * @brief Enriches a Game object with its genres from Steam API.
 *
 * Only fetches genres if the Game does not already have them.
 *
 * @param game The Game object to enrich.
 */
void ApiClient::enrichGameWithGenres(Game &game)
{
    if (!game.genres.empty())
        return;
    game.genres = fetchGenres(game.appid);
}

/**
 * @brief Enriches multiple Game objects with genres.
 *
 * Iterates over all games and calls enrichGameWithGenres for each.
 *
 * @param games Vector of Game objects to enrich.
 */
void ApiClient::enrichGamesParallel(std::vector<Game> &games, size_t max_concurrent)
{
    std::vector<std::future<void>> future;
    for (size_t i = 0; i < games.size(); ++i)
    {
        try {
            future.push_back(std::async(std::launch::async, &ApiClient::enrichGameWithGenres, this, std::ref(games[i])));
        }
        catch (const std::exception &e) {
            logger("Something went wrong! [" + games[i].appid + " " + std::string(e.what()) + "]");
        }

        if (future.size() >= max_concurrent)
        {
            for(auto &f : future) f.get();
            future.clear();
        }
    }
    for (auto &f : future) f.get();
}

/**
 * @brief Performs an HTTP GET request to the specified URL.
 *
 * Uses libcurl to fetch content from the given URL. Follows redirects
 * and sets a 10-second timeout.
 *
 * @param rUrl The URL to fetch.
 * @return A string containing the response body. Empty if the request fails.
 */
std::string ApiClient::httpGet(const std::string &rUrl) {
    CURL* curl = curl_easy_init();
    std::string response;

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, rUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return response;
}

void ApiClient::saveToCache(std::string appid, const std::unordered_set<std::string> &genres)
{
    std::string app_path = cache_path + appid + ".json";
    std::ostringstream oss;
    json j_genres;

    std::ofstream file(app_path);
    if (file.is_open())
    {
        to_json(j_genres, genres);
        file << j_genres.dump(4);
    }
    else
    {
        {
            logger("[CACHE FILE ERROR] " + appid);
        }
    }
}

std::unordered_set<std::string> ApiClient::loadFromCache(std::string appid)
{
    std::unordered_set<std::string> genres;
    json j;
    std::ifstream file(cache_path + appid + ".json");
    if(file.is_open()) {
        try {
            file >> j;
            from_json(j, genres);
            return genres;
        }
        catch (const std::exception &e) {
            logger("[CACHE FAIL] " + appid + std::string(e.what()));
            return genres;
        }
    }
    return genres;
}

void ApiClient::logger(const std::string &log)
{
    std::ostringstream oss;

    oss << log << std::endl;
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << oss.str();
}
