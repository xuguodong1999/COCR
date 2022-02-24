project(tutorials-qt)

set(QT_COMPONENTS Core)
find_package(QT NAMES Qt6 Qt5 COMPONENTS ${QT_COMPONENTS} QUIET)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENTS} QUIET)

set(LIB_ALL_FOUND ON)
foreach (QT_COMPONENT ${QT_COMPONENTS})
    if (NOT Qt${QT_VERSION_MAJOR}${QT_COMPONENT}_FOUND)
        set(LIB_ALL_FOUND OFF)
        message(WARNING Qt${QT_VERSION_MAJOR}${QT_COMPONENT} " not found in " ${QT_DIR})
    endif ()
endforeach (QT_COMPONENT ${QT_COMPONENTS})

if (NOT ${LIB_ALL_FOUND})
    message(WARNING "code in " ${CMAKE_CURRENT_SOURCE_DIR} " will not be built")
else ()
    set(CMAKE_AUTORCC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCE)
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCE})

    set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

    set(QT_COMPONENT_TARGETS)
    foreach (QT_COMPONENT ${QT_COMPONENTS})
        list(APPEND QT_COMPONENT_TARGETS Qt${QT_VERSION_MAJOR}::${QT_COMPONENT})
    endforeach (QT_COMPONENT ${QT_COMPONENTS})

    target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_COMPONENT_TARGETS})
endif ()
