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

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(vinter-engine)
add_subdirectory(vinter-editor)
add_subdirectory(game-bomberman)

######################################################################################################################
# Platform and Compiler settings
######################################################################################################################
add_compile_options(-Wall -Wextra -Wpedantic)