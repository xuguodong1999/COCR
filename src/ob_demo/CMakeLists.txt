project(ob_demo CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTORCC ON)

set(RES_SRCS ${RES_ROOT}/obdata.qrc)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)

# include dir
include_directories(${3RD_LIB_DIR}/openbabel/include ./)

# source dir
aux_source_directory(./ PROJECT_SOURCES)

list(APPEND PROJECT_SOURCES ${RES_SRCS})

if (MSVC)
    list(APPEND PROJECT_SOURCES ${RES_ROOT}/leafxy.rc)
    include_directories(${RES_ROOT})
endif (MSVC)

#set_source_files_properties(${PROJECT_SOURCES} PROPERTIES LANGUAGE CXX)

# add exe
if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ob_demo ${PROJECT_SOURCES})
else ()
    if (ANDROID)
        add_library(ob_demo SHARED ${PROJECT_SOURCES})
    else ()
        add_executable(ob_demo ${PROJECT_SOURCES})
    endif ()
endif ()

# product suffix
set_target_properties(ob_demo PROPERTIES DEBUG_POSTFIX "d")

# for future qml cases
target_compile_definitions(ob_demo PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

# link opencv, patched openbabel and qt here
target_link_libraries(ob_demo PRIVATE
        openbabel
        Qt${QT_VERSION_MAJOR}::Core)
