#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <future>

#include <iostream>

#include "api_client.h"

void to_json(nlohmann::json& j, const std::unordered_set<std::string> &genres)
{
    j = nlohmann::json{
        {"genres", genres}
    };
}
void from_json(const nlohmann::json &j, std::unordered_set<std::string> &genres)
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
 * @brief Performs an HTTP GET request to the specified URL.
 *
 * Uses libcurl to fetch content from the given URL. Follows redirects
 * and sets a 10-second timeout.
 *
 * @param rUrl The URL to fetch.
 * @return A string containing the response body. Empty if the request fails.
 */
std::string httpGet(const std::string &rUrl) {
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

using json = nlohmann::json;

/**
 * @brief Fetches the list of genres for a given Steam app.
 *
 * Queries the Steam Store API for the app's details and extracts
 * genre descriptions.
 *
 * @param appid The Steam application ID.
 * @return A vector of genre names. Empty if none are found or request fails.
 */
std::unordered_set<std::string> fetchGenres(int appid)
{
    std::string app_path = "cache/apps/" + std::to_string(appid) + ".json";
    std::filesystem::create_directories(std::filesystem::path(app_path).parent_path());

    std::mutex cout_mutex;
    std::ostringstream oss;

    if (std::filesystem::exists(app_path))
    {
        json j;
        std::ifstream file(app_path);
        if(file.is_open()) {
            try {
                file >> j;
                std::unordered_set<std::string> genres;
                from_json(j, genres);
                {
                    oss << "[CACHE HIT] " << appid << std::endl;
                    std::lock_guard<std::mutex> lock(cout_mutex);
                    std::cout << oss.str();
                }
                return genres;
            }
            catch (const std::exception &e) {
                std::cout << "Something went wrong! " << e.what() << std::endl;
            }
        }
    }
    {
        oss << "[API FETCH] " << std::to_string(appid) << std::endl;
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << oss.str();
    }
    std::string url =
        "https://store.steampowered.com/api/appdetails?appids=" +
        std::to_string(appid) + "&l=en";

    std::unordered_set<std::string> genres;
    // Call Steam API
    std::string response = httpGet(url);
    if (response.empty()) {
        {
            oss << "[API FETCH] " << std::to_string(appid) << std::endl;
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "[API FAIL]" << std::endl;
        }
        return genres;
    }

    // Parse Steam response
    try {
        json j_response = json::parse(response);

        std::string appid_str = std::to_string(appid);

        if (j_response.contains(appid_str) &&
            j_response[appid_str]["success"] == true &&
            j_response[appid_str]["data"].contains("genres"))
        {
            for (const auto &g : j_response[appid_str]["data"]["genres"])
            {
                genres.insert(g["description"]);
            }
        }
        else {
            {
                oss << "[CACHE WRITE EMPTY] " << std::to_string(appid) << std::endl;
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << oss.str();
            }
        }
    } catch (const std::exception &e) {
        {
            oss << "Something went wrong! " << e.what() << std::endl;
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << oss.str();
        }
    }

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
            oss << "[CACHE FILE ERROR]" << std::endl;
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << oss.str();
        }
    }
    return genres;
}

/**
 * @brief Enriches a Game object with its genres from Steam API.
 *
 * Only fetches genres if the Game does not already have them.
 *
 * @param game The Game object to enrich.
 */
void enrichGameWithGenres(Game &game)
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
void enrichGamesParallel(std::vector<Game> &games, size_t max_concurrent)
{
    std::vector<std::future<void>> future;
    for (size_t i = 0; i < games.size(); ++i)
    {
        try {
            future.push_back(std::async(std::launch::async, enrichGameWithGenres, std::ref(games[i])));
        }
        catch (const std::exception e) {
            std::cout << "Something failed! " << e.what() << std::endl;
            std::cout << "Happened to game: [" << games[i].name << "] [" << games[i].appid << "]" << std::endl;
        }

        if (future.size() >= max_concurrent)
        {
            for(auto &f : future) f.get();
            future.clear();
        }
    }
    for (auto &f : future) f.get();
}