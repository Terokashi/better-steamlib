set(HEADERS
    include/steam_parser.h
    include/cache.h
    include/api_client.h
    include/game.h
    include/igamelibrary.h
    include/gamemanager.h
)

add_library(steamcore STATIC
    src/steam_parser.cpp
    src/cache.cpp
    src/api_client.cpp
    src/gamemanager.cpp
    ${HEADERS}
)

target_include_directories(steamcore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# vcpkg CURL
find_package(CURL CONFIG REQUIRED)

target_link_libraries(steamcore PUBLIC CURL::libcurl)