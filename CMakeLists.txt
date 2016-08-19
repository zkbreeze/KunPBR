cmake_minimum_required(VERSION 2.8)
project(KunPBR)

# KVS Library
set(KVS_DIR $ENV{KVS_DIR})
include_directories( ${KVS_DIR}/include )
link_directories( ${KVS_DIR}/lib )

# Adv Library
set(Adv_DIR $ENV{Adv_DIR})
include_directories( ${Adv_DIR}/include )
link_directories( ${Adv_DIR}/lib )

option(DEBUG "DEBUG" OFF)

if(DEBUG)
	set(CMAKE_BUILD_TYPE "Debug")
	add_definitions(-ggdb3)
	add_definitions(-DKVS_ENABLE_DEBUG)
	add_definitions(-D_DEBUG)
endif()

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -I${GLEW_INCLUDE_DIRS}")

# Library
add_subdirectory(include)
include_directories(${PROJECT_SOURCE_DIR}/include)

# Build directory
add_subdirectory(App)
add_subdirectory(CorrelationApp)

# INSTALL(
# 	TARGETS kunPBR
# 	ARCHIEVE DESTINATION ${PROJECT_SOURCE_DIR}/lib
# 	 )