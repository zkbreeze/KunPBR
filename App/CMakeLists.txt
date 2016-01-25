cmake_minimum_required(VERSION 2.8)

# Build Source
add_executable(PBR PBR/main.cpp )
target_link_libraries(PBR kunPBR kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(AnimationPBR AnimationPBR/main.cpp )
target_link_libraries(AnimationPBR kunPBR kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(AnimationRay AnimationRay/main.cpp )
target_link_libraries(AnimationRay kunPBR kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(ImageEvaluation ImageEvaluation/main.cpp )
target_link_libraries(ImageEvaluation kunPBR kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(OldPBVR OldPBVR/main.cpp )
target_link_libraries(OldPBVR kunPBR kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(PointPBR PointPBR/main.cpp )
target_link_libraries(PointPBR kunPBR kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(Tsunami Tsunami/main.cpp )
target_link_libraries(Tsunami kunPBR kvsCore kvsSupportGLUT AdvBase AdvDocIO AdvFileIO ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(AnimationTsunami AnimationTsunami/main.cpp )
target_link_libraries(AnimationTsunami kunPBR kvsCore kvsSupportGLUT AdvBase AdvDocIO AdvFileIO ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(Cloud Cloud/main.cpp )
target_link_libraries(Cloud kunPBR kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(AnimationCloud AnimationCloud/main.cpp )
target_link_libraries(AnimationCloud kunPBR kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})