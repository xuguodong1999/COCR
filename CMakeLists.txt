cmake_minimum_required(VERSION 3.16)
project(TestModuleImport)
set(CMAKE_CXX_STANDARD 14)

set(OpenCV_DIR "lib/opencv/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS}/opencv2)

# # OpenBabel3 Support
# set(OpenBabel3_DIR "/home/xgd/3rdparty/openbabel/lib/cmake/openbabel3")
# find_package(OpenBabel3 REQUIRED)
# include_directories(${OpenBabel3_INCLUDE_DIRS})

# # Frugally-deep Support
# include_directories(
#         "/home/xgd/source/frugally-deep/include"
#         "/home/xgd/source/FunctionalPlus/include"
#         "/home/xgd/source/json/include"
# )

# # Eigen3 Support
# # set(Eigen3_DIR "/home/xgd/3rdparty/eigen3/share/eigen3/cmake")
# find_package(Eigen3 REQUIRED NO_MODULE)
# include_directories(${EIGEN3_INCLUDE_DIR})

aux_source_directory(src DIR_SRCS)
add_executable(${PROJECT_NAME} ${DIR_SRCS})

target_link_libraries(
        ${PROJECT_NAME}
        ${OpenCV_LIBS}
)