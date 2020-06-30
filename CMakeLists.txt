cmake_minimum_required(VERSION 3.16)
project(TestModuleImport)
set(CMAKE_CXX_STANDARD 14)

set(OpenCV_DIR "lib/opencv/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS}/opencv2)

# OpenBabel2 Support
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
set(OpenBabel2_DIR "lib/openbabel/lib/cmake/openbabel2")
set(OPENBABEL2_INCLUDE_DIR lib/openbabel/include/openbabel-2.0)
find_package(OpenBabel2 REQUIRED)
include_directories(${OPENBABEL2_INCLUDE_DIR})

# Eigen3 Support
set(Eigen3_DIR "lib/eigen/share/eigen3/cmake")
find_package(Eigen3 REQUIRED NO_MODULE)
include_directories(${EIGEN3_INCLUDE_DIR})


aux_source_directory(src DIR_SRCS)
add_executable(${PROJECT_NAME} ${DIR_SRCS})

target_link_libraries(
        ${PROJECT_NAME}
        ${OpenCV_LIBS}
        ${OPENBABEL2_LIBRARIES}
)