project(maeparser LANGUAGES CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src PROJECT_SOURCE)

set(QT_COMPONENTS Core)
find_package(QT NAMES Qt6 Qt5 COMPONENTS ${QT_COMPONENTS} REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENTS} REQUIRED)

find_package(Boost COMPONENTS iostreams REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
#link_directories(${Boost_LIB_DIR})

add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

set(QT_COMPONENT_TARGETS)
foreach (QT_COMPONENT ${QT_COMPONENTS})
    list(APPEND QT_COMPONENT_TARGETS Qt${QT_VERSION_MAJOR}::${QT_COMPONENT})
endforeach ()

target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_COMPONENT_TARGETS}
        # ${Boost_LIBRARIES}
        )

target_compile_definitions(${PROJECT_NAME} PRIVATE MAEPARSER_LIBRARY)