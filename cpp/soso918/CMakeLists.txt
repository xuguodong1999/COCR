set(MODULE_NAME soso918)

if (MSVC)
    set(OpenBabel_DIR C:/Users/xgd/install/openbabel)
    if (CMAKE_BUILD_TYPE)
        if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
            set(OpenBabel_LIBS ${OpenBabel_DIR}/staticlibs/openbabel-3d.lib)
            endif ()
    else ()
        set(OpenBabel_LIBS ${OpenBabel_DIR}/staticlibs/openbabel-3.lib)
    endif ()
elseif (UNIX)
    set(OpenBabel_DIR /home/xgd/install/openbabel)
    set(OpenBabel_LIBS ${OpenBabel_DIR}/lib/libopenbabel.so)
endif ()


aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)

add_executable(${MODULE_NAME} ${SRC_FILES})

if (MSVC)
    if(CMAKE_BUILD_TYPE)
        if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
            target_compile_options(${MODULE_NAME} PRIVATE /MTd)
        endif ()
    else ()
        target_compile_options(${MODULE_NAME} PRIVATE /MT)
    endif ()
elseif (UNIX)
endif ()

target_link_libraries(${MODULE_NAME} PRIVATE ${OpenCV_LIBS} ${OpenBabel_LIBS})
