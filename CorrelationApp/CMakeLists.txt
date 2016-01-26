cmake_minimum_required(VERSION 2.8)

# Build Source
add_executable(CheckCorrelation CheckCorrelation/main.cpp )
target_link_libraries(CheckCorrelation kunPBR kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(CSVCorrelationVolume CSVCorrelationVolume/main.cpp )
target_link_libraries(CSVCorrelationVolume kunPBR kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(VolumeCorrelation VolumeCorrelation/main.cpp )
target_link_libraries(VolumeCorrelation kunPBR kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(OutputPoint OutputPoint/main.cpp )
target_link_libraries(OutputPoint kunPBR kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

add_executable(HeatSphereCorr HeatSphereCorr/main.cpp )
target_link_libraries(HeatSphereCorr kunPBR kvsCore kvsSupportGLUT ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})