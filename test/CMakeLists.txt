set(MODULE_NAME COCR-test)

enable_testing()
set(GTEST_ROOT ${CMAKE_SOURCE_DIR}/lib/googletest)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

# FIXME: linux下修改到Qt库的cmake文件夹位置
set(Qt5_DIR /home/xgd/Qt/5.15.0/gcc_64/lib/cmake/Qt5)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# 配置 OpenCV
set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/lib/opencv/lib/cmake/opencv4)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
# 配置 OpenBabel
set(OPENBABEL2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lib/openbabel/include/openbabel3)
set(OPENBABEL2_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/openbabel/lib/libopenbabel.so)
include_directories(${OPENBABEL2_INCLUDE_DIR})
# 配置 Eigen3
set(Eigen3_DIR ${CMAKE_SOURCE_DIR}/lib/eigen/share/eigen3/cmake)
find_package(Eigen3 REQUIRED NO_MODULE)
include_directories(${EIGEN3_INCLUDE_DIR})
# 配置 FrugallyDeep
include_directories(${CMAKE_SOURCE_DIR}/lib/json/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/FunctionalPlus/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/frugally-deep/include)
# 配置 RapidJson
include_directories(${CMAKE_SOURCE_DIR}/lib/rapidjson/include)
# 自己的代码
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(${MODULE_NAME} test.cpp) # TODO: 手动添加参与测试的cpp
target_link_libraries(${MODULE_NAME}
        ${GTEST_ROOT}/lib/libgtest.a
        ${GTEST_ROOT}/lib/libgtest_main.a
        ${OPENBABEL2_LIBRARIES}
        ${OpenCV_LIBS}
        Qt5::Widgets
        pthread)
add_test(NAME ${MODULE_NAME} COMMAND ${MODULE_NAME})
