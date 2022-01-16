project(cocr-simple)

include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenMP.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenCV.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupTorch.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupQt5.cmake)

set(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/play_torch.cpp
        ${SRC_DIR}/opencvutil.cpp
        ${SRC_DIR}/fontpixitem.cpp
        ${SRC_DIR}/gb2312data.cpp
        ${SRC_DIR}/config.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Widgets
        ${OpenCV_LIBS}
        ${TORCH_LIBRARIES})

