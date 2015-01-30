cmake_minimum_required(VERSION 2.8)
project(KunPBR)
set($ENV{KUN_SHADER_DIR} "${PROJECT_SOURCE_DIR}/lib/Shader/")

set(KVS_DIR $ENV{KVS_DIR})
include_directories( ${KVS_DIR}/include )
link_directories( ${KVS_DIR}/lib )

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${GLEW_INCLUDE_DIRS}")

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


# INSTALL(
# 	TARGETS util
# 	ARCHIEVE DESTINATION ${PROJECT_SOURCE_DIR}/lib
# 	 )