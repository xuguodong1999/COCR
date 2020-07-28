set(MODULE_NAME COCR-test)

enable_testing()
set(GTEST_ROOT ${CMAKE_SOURCE_DIR}/lib/googletest)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

# 自己的代码
include_directories(${CMAKE_SOURCE_DIR}/include)

aux_source_directory(${CMAKE_SOURCE_DIR}/test DIR_SRCS)

add_executable(${MODULE_NAME} ${DIR_SRCS} ${QT_RES_SRC})
add_dependencies(${MODULE_NAME} res_qrc)

if (UNIX)
    target_link_libraries(${MODULE_NAME}
#            ${QT_BIG_RES_SRC}
            ${GTEST_ROOT}/lib/libgtest.a
            ${GTEST_ROOT}/lib/libgtest_main.a
            ${OPENBABEL2_LIBRARIES}
            ${OpenCV_LIBS}
            Qt5::Widgets
            pthread)
elseif (MSVC)
    if (CMAKE_BUILD_TYPE)
        if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
            target_compile_options(${MODULE_NAME} PRIVATE /MTd)
        else ()
            target_compile_options(${MODULE_NAME} PRIVATE /MT)
        endif ()
    else ()
        target_compile_options(${MODULE_NAME} PRIVATE /MT)
    endif ()
    target_link_libraries(${MODULE_NAME}
#            ${QT_BIG_RES_SRC}
            ${GTEST_ROOT}/lib/gtest.lib
            ${GTEST_ROOT}/lib/gtest_main.lib
            ${OPENBABEL2_LIBRARIES}
            ${OpenCV_LIBS}
            Qt5::Widgets)
endif ()

add_test(NAME ${MODULE_NAME} COMMAND ${MODULE_NAME})