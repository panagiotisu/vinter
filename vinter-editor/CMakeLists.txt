cmake_minimum_required(VERSION 3.28)
project(vinter-editor LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.92.9b-docking
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    # SDL3 + SDL_Renderer backends
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

target_include_directories(imgui
    PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

# Backends need SDL3 headers; SDL3::SDL3 is already fetched by vinter-engine.
target_link_libraries(imgui PUBLIC SDL3::SDL3)

######################################################################################################################
# Editor Executable
######################################################################################################################
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC asan_flags)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        vinter-engine
        imgui
)

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
