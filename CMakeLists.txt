cmake_minimum_required(VERSION 3.16)
project(TestModuleImport)
set(CMAKE_CXX_STANDARD 14)

if(UNIX)
set(OpenCV_DIR "lib/opencv/lib/cmake/opencv4")
elseif(MSVC)
set(OpenCV_DIR "lib/opencv/x64/vc16/lib")
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS}/opencv2)

# OpenBabel2 Support
set(OPENBABEL2_INCLUDE_DIR lib/openbabel/include/openbabel3)
set(OPENBABEL2_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/openbabel/bin/openbabel-3.lib)
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