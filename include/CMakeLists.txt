cmake_minimum_required(VERSION 2.8)

file(GLOB_RECURSE  Library "*.cpp")
add_library(kunPBR ${Library})
set_target_properties(kunPBR PROPERTIES
         ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)