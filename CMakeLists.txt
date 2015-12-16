cmake_minimum_required(VERSION 2.8)
project(KunPBR)
set($ENV{KUN_SHADER_DIR} "${PROJECT_SOURCE_DIR}/lib/Shader/")

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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${GLEW_INCLUDE_DIRS}")

# Build Library
file(GLOB_RECURSE  Library "${PROJECT_SOURCE_DIR}/lib/*.cpp")
add_library(util ${Library})
include_directories(${PROJECT_SOURCE_DIR}/lib)

# Build Source
add_executable(PBR ${PROJECT_SOURCE_DIR}/App/PBR/main.cpp )
target_link_libraries(PBR util kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(AnimationPBR ${PROJECT_SOURCE_DIR}/App/AnimationPBR/main.cpp )
target_link_libraries(AnimationPBR util kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(AnimationRay ${PROJECT_SOURCE_DIR}/App/AnimationRay/main.cpp )
target_link_libraries(AnimationRay util kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(ImageEvaluation ${PROJECT_SOURCE_DIR}/App/ImageEvaluation/main.cpp )
target_link_libraries(ImageEvaluation util kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(OldPBVR ${PROJECT_SOURCE_DIR}/App/OldPBVR/main.cpp )
target_link_libraries(OldPBVR util kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(PointPBR ${PROJECT_SOURCE_DIR}/App/PointPBR/main.cpp )
target_link_libraries(PointPBR util kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(Tsunami ${PROJECT_SOURCE_DIR}/App/Tsunami/main.cpp )
target_link_libraries(Tsunami util kvsCore kvsSupportGLUT AdvBase AdvDocIO AdvFileIO ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(AnimationTsunami ${PROJECT_SOURCE_DIR}/App/AnimationTsunami/main.cpp )
target_link_libraries(AnimationTsunami util kvsCore kvsSupportGLUT AdvBase AdvDocIO AdvFileIO ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(Cloud ${PROJECT_SOURCE_DIR}/App/Cloud/main.cpp )
target_link_libraries(Cloud util kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(AnimationCloud ${PROJECT_SOURCE_DIR}/App/AnimationCloud/main.cpp )
target_link_libraries(AnimationCloud util kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(CorrelationVis ${PROJECT_SOURCE_DIR}/App/CorrelationVis/main.cpp )
target_link_libraries(CorrelationVis util kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})


# INSTALL(
# 	TARGETS util
# 	ARCHIEVE DESTINATION ${PROJECT_SOURCE_DIR}/lib
# 	 )