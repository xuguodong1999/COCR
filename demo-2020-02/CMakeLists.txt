set(MODULE_NAME COCR-demo-2020-02)

# 自己的代码
include_directories(${CMAKE_SOURCE_DIR}/demo-2020-02)
aux_source_directory(${CMAKE_SOURCE_DIR}/demo-2020-02 DIR_SRCS)

qt5_add_resources(DIR_SRCS ../res/img.qrc ../res/obabel.qrc)
qt5_add_big_resources(DIR_SRCS ../res/weights.qrc)

add_executable(${MODULE_NAME} ${DIR_SRCS})

# 修改MSVC为静态编译
if (MSVC)
    if (CMAKE_BUILD_TYPE)
        if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
            target_compile_options(${MODULE_NAME} PRIVATE /MTd)
        else ()
            target_compile_options(${MODULE_NAME} PRIVATE /MT)
        endif ()
    else ()
        target_compile_options(${MODULE_NAME} PRIVATE /MT)
    endif ()
endif ()

target_link_libraries(${MODULE_NAME}
        ${OPENBABEL2_LIBRARIES}
        ${OpenCV_LIBS}
        Qt5::Widgets)
