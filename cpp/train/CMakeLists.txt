project(cocr-train)

if (UNIX)
    set(Torch_DIR /home/xgd/install/libtorch-latest/share/cmake/Torch)
endif (UNIX)


include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenMP.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenCV.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupTorch.cmake)

set(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${SRC_DIR}/config.cpp
        ${SRC_DIR}/block.cpp
        ${SRC_DIR}/activation.cpp
        ${SRC_DIR}/module.cpp
        ${SRC_DIR}/mobilenetv3.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OpenCV_LIBS}
        ${TORCH_LIBRARIES})
