cmake_minimum_required(VERSION 3.28)
project(bomberman LANGUAGES CXX)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# Game also links against engine
target_link_libraries(${PROJECT_NAME} PRIVATE vinter-engine)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src/")