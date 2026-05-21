set(HEADERS
	include/cli_parser.h
	include/cli_types.h
	include/printer.h
	include/style.h
)
add_executable(better-steamlibcli
    src/main.cpp
    src/printer.cpp
    src/cli_parser.cpp
    src/cli_types.cpp
	
	${HEADERS}
)

target_include_directories(better-steamlibcli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(better-steamlibcli PRIVATE
    steamcore
)