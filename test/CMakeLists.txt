set(MODULE_NAME COCR-test)

enable_testing()
set(GTEST_ROOT ${CMAKE_SOURCE_DIR}/lib/googletest)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

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
