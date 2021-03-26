project(coordgen)

set(CMAKE_CXX_STANDARD 17)

if(NOT ANDROID)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
endif(NOT ANDROID)

set(COORDGEN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${COORDGEN_SRC_DIR}/include ${COORDGEN_SRC_DIR}/include/coordgen)

aux_source_directory(${COORDGEN_SRC_DIR}/src COORDGEN_SRCS)

add_library(${PROJECT_NAME} SHARED ${COORDGEN_SRCS})

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

# link qt core here
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)
