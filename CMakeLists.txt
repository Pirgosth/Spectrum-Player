cmake_minimum_required(VERSION 3.12)
find_package(PkgConfig REQUIRED)

project(Spectrum VERSION 2.0 DESCRIPTION "FFT Spectrum Mp3 player written in C++" LANGUAGES CXX)

find_package(SFML 2 COMPONENTS system graphics audio REQUIRED)
pkg_check_modules(MPG123 libmpg123>=1.33.1 REQUIRED)
pkg_check_modules(OUT123 libout123>=1.33.1 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(${CMAKE_PROJECT_NAME})

set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_link_libraries(${CMAKE_PROJECT_NAME} ${MPG123_LIBRARIES} ${OUT123_LIBRARIES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${MPG123_INCLUDE_DIRS} ${OUT123_INCLUDE_DIRS})
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic ${MPG123_CFLAGS_OTHER} ${OUT123_CFLAGS_OTHER})

# if (WIN32)
    # target_link_libraries(${CMAKE_PROJECT_NAME} libECSd.a sfml-system sfml-window sfml-graphics sfml-audio nlohmann_json::nlohmann_json)
# else()
    target_link_libraries(${CMAKE_PROJECT_NAME} sfml-system sfml-window sfml-graphics sfml-audio mpg123)
# endif()

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/Resources $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/Resources)

target_sources(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mp3AudioStream.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mp3Handler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils.cpp
    ${CMAKE_CURRENT_LIST_DIR}/visuals.cpp
)