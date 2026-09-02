cmake_minimum_required(VERSION 3.28)
project(bomberman LANGUAGES CXX)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC asan_flags)

# Game also links against engine
target_link_libraries(${PROJECT_NAME} PRIVATE vinter-engine)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src/")

# Bundle shared library dlls with executable on Windows.
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
    )
endif()