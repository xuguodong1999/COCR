set(MODULE_NAME COCR-app)

# 自己的代码
include_directories(${CMAKE_SOURCE_DIR}/include)
aux_source_directory(${CMAKE_SOURCE_DIR}/src DIR_SRCS)

add_executable(${MODULE_NAME} ${DIR_SRCS} ${QT_RES_SRC})
add_dependencies(${MODULE_NAME} res_qrc)

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

target_link_libraries(${MODULE_NAME} PRIVATE
#        ${QT_BIG_RES_SRC}
        ${OPENBABEL2_LIBRARIES}
        ${OpenCV_LIBS}
        Qt5::Widgets)
