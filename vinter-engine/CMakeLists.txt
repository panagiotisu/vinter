######################################################################################################################
# Project
######################################################################################################################
cmake_minimum_required(VERSION 3.28)
project(vinter-engine VERSION 0.1.0 LANGUAGES CXX)

######################################################################################################################
# Dependencies
######################################################################################################################
include(FetchContent)

function(include_dependency libName gitURL gitTag)
    FetchContent_Declare(${libName}
        GIT_REPOSITORY ${gitURL}
        GIT_TAG        ${gitTag}
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
    )
    FetchContent_MakeAvailable(${libName})
endfunction()

# --- SDL3 ---
# Shared build (default is usually shared if not forced static)
set(SDL_SHARED ON  CACHE BOOL "Build SDL3 as shared" FORCE)
set(SDL_STATIC OFF CACHE BOOL "Disable static SDL3" FORCE)
include_dependency(SDL3 https://github.com/libsdl-org/SDL.git release-3.4.0)

# --- SDL3_ttf ---
set(SDLTTF_VENDORED ON)
include_dependency(SDL3_ttf https://github.com/libsdl-org/SDL_ttf.git release-3.2.2)

# --- EnTT ECS ---
set(ENTT_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ENTT_BUILD_DOCS OFF CACHE BOOL "" FORCE)
include_dependency(EnTT https://github.com/skypjack/entt.git v3.15.0)

# --- glm Math ---
include_dependency(glm https://github.com/g-truc/glm.git 1.0.2)

######################################################################################################################
# Library
######################################################################################################################
add_library(${PROJECT_NAME} SHARED)

file(GLOB PUBLIC_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_LIST_DIR}/include/vinter/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/include/vinter/*.hpp"
)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/src/*.hpp"
)

target_sources(${PROJECT_NAME}
    PUBLIC
        FILE_SET public_headers
        TYPE HEADERS
        BASE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include/"
        FILES ${PUBLIC_HEADERS}
    PRIVATE
        ${PROJECT_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        EnTT::EnTT  # Very opinionated, best be exposed.
        glm::glm    # Fundamental, no reason to hide.
    PRIVATE
        SDL3::SDL3
        SDL3_ttf::SDL3_ttf
)