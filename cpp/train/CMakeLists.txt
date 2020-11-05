project(cocr-train)

include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenMP.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenCV.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupTorch.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupQt5.cmake)

set(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${SRC_DIR}/activation.cpp
        ${SRC_DIR}/sosodataset.cpp
        ${SRC_DIR}/couch.cpp
        ${SRC_DIR}/shapeitem.cpp
        ${SRC_DIR}/statistic.cpp
        ${SRC_DIR}/couchdataset.cpp
        ${SRC_DIR}/opencvutil.cpp
        ${SRC_DIR}/config.cpp
        ${SRC_DIR}/block.cpp
        ${SRC_DIR}/activation.cpp
        ${SRC_DIR}/mv3_large.cpp
        ${SRC_DIR}/mobilenetv3.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OpenCV_LIBS}
        ${TORCH_LIBRARIES}
        Qt5::Widgets)
