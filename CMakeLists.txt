######################################################################################################################
# Project
######################################################################################################################
cmake_minimum_required(VERSION 3.28)
project(vinter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# In case of QTCreator.
set(QT_CREATOR_SKIP_MAINTENANCE_TOOL_PROVIDER ON CACHE INTERNAL "")

# Address Sanitizing (ASan).
add_library(project_asan_flags INTERFACE)
if(MSVC)
    target_compile_options(project_asan_flags INTERFACE
        $<$<CONFIG:Debug>:/fsanitize=address>
    )
    # MSVC ASan requires turning off incremental linking.
    target_link_options(project_asan_flags INTERFACE
        $<$<CONFIG:Debug>:/fsanitize=address;/INCREMENTAL:NO>
    )
else() # GCC / Clang (Linux, macOS, MinGW).
    target_compile_options(project_asan_flags INTERFACE
        $<$<CONFIG:Debug>:-fsanitize=address;-fno-omit-frame-pointer>
    )
    target_link_options(project_asan_flags INTERFACE
        $<$<CONFIG:Debug>:-fsanitize=address>
    )
endif()


add_subdirectory("vinter-engine")
add_subdirectory("vinter-editor")
add_subdirectory("examples/bomberman")
add_subdirectory("examples/gravitas")

######################################################################################################################
# Platform and Compiler settings
######################################################################################################################
add_compile_options(-Wall -Wextra -Wpedantic)