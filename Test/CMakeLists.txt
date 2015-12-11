cmake_minimum_required(VERSION 2.8)
project(KunPBR_Test)
set($ENV{KUN_SHADER_DIR} "${PROJECT_SOURCE_DIR}/../lib/Shader/")

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
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${GLEW_INCLUDE_DIRS}")

# Build Library
file(GLOB_RECURSE  Library "${PROJECT_SOURCE_DIR}/../lib/*.cpp")
add_library(util ${Library})
include_directories(${PROJECT_SOURCE_DIR}/../lib)

# Build Source
add_executable(OBJLoader ${PROJECT_SOURCE_DIR}/OBJLoader/main.cpp )
target_link_libraries(OBJLoader util kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(OpenGLPointRendering ${PROJECT_SOURCE_DIR}/OpenGLPointRendering/main.cpp )
target_link_libraries(OpenGLPointRendering util kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

# add_executable(PolygonClip ${PROJECT_SOURCE_DIR}/App/PolygonClip/main.cpp )
# target_link_libraries(PolygonClip util kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(BloodPolygonRendering ${PROJECT_SOURCE_DIR}/BloodPolygonRendering/main.cpp )
target_link_libraries(BloodPolygonRendering util kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(CheckCorrelation ${PROJECT_SOURCE_DIR}/CheckCorrelation/main.cpp )
target_link_libraries(CheckCorrelation util kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(CorrelationVolume ${PROJECT_SOURCE_DIR}/CorrelationVolume/main.cpp )
target_link_libraries(CorrelationVolume util kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})


# INSTALL(
# 	TARGETS util
# 	ARCHIEVE DESTINATION ${PROJECT_SOURCE_DIR}/lib
# 	 )