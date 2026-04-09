cmake_minimum_required(VERSION 3.16)

project(better_steamlib VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Packages via vcpkg ---
find_package(CURL REQUIRED)

# --- Sources ---
set(SOURCES
    src/main.cpp
    src/steam_parser.cpp
    src/printer.cpp
    src/cache.cpp
    src/cli_parser.cpp
    src/cli_types.cpp
    src/api_client.cpp

)

# --- Headers (for IDE visibility) ---
set(HEADERS
    include/game.h
    include/steam_parser.h
    include/printer.h
    include/cache.h
    include/cli_parser.h
    include/cli_types.h
    include/api_client.h
    include/style.h
    include/igamelibrary.h
)

# --- Executable ---
add_executable(better-steamlib ${SOURCES} ${HEADERS})

# --- Include dirs ---
target_include_directories(better-steamlib PRIVATE
    include
)

# --- Link libraries ---
target_link_libraries(better-steamlib PRIVATE
    CURL::libcurl
)

# --- Optional: warnings ---
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()