project(openbabel LANGUAGES CXX)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/inchi)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../maeparser/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../global_header)

add_definitions(-DWITH_INCHI)
add_definitions(-DWITH_JSON)
add_definitions(-DWITH_COORDGEN)
add_definitions(-DHAVE_EIGEN)
add_definitions(-DHAVE_EIGEN3)
add_definitions(-DMAKE_OBDLL)

set(PROJECT_SOURCE_DIR_LIST
        src
        src/charges
        src/depict
        src/descriptors
        src/fingerprints
        src/forcefields
        src/formats
        src/formats/json
        src/formats/libinchi
        src/math
        src/ops
        src/stereo)
set(PROJECT_SOURCE)
foreach (PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR_LIST})
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_SOURCE_DIR} PROJECT_SOURCES)
    list(APPEND PROJECT_SOURCE ${PROJECT_SOURCES})
endforeach ()

set(QT_COMPONENTS Core)
find_package(QT NAMES Qt6 Qt5 COMPONENTS ${QT_COMPONENTS} REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENTS} REQUIRED)

find_package(Boost COMPONENTS iostreams REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIB_DIR})

# generate openbabel qrc
set(DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../data)
set(OPENBABEL_DATA_DIR ${DATA_DIR}/openbabel)
file(GLOB_RECURSE OPENBABEL_DATA_LIST ${OPENBABEL_DATA_DIR}/**)
set(OPENBABEL_QRC "")
string(APPEND OPENBABEL_QRC "<RCC>\n<qresource prefix=\"/\">\n")
foreach (OPENBABEL_DATA ${OPENBABEL_DATA_LIST})
    string(REPLACE ${DATA_DIR}/ "" OPENBABEL_DATA_RELATIVE_PATH ${OPENBABEL_DATA})
    string(APPEND OPENBABEL_QRC
            "<file compress=\"9\" compress-algo=\"best\">${OPENBABEL_DATA_RELATIVE_PATH}</file>\n")
endforeach ()
string(APPEND OPENBABEL_QRC "</qresource>\n</RCC>")
set(OPENBABEL_DATA_QRC_FILE ${OPENBABEL_DATA_DIR}/../openbabel_data.qrc)
file(WRITE ${OPENBABEL_DATA_QRC_FILE} ${OPENBABEL_QRC})

add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCE} ${OPENBABEL_DATA_QRC_FILE})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

set(QT_COMPONENT_TARGETS)
foreach (QT_COMPONENT ${QT_COMPONENTS})
    list(APPEND QT_COMPONENT_TARGETS Qt${QT_VERSION_MAJOR}::${QT_COMPONENT})
endforeach (QT_COMPONENT ${QT_COMPONENTS})

target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_COMPONENT_TARGETS} coordgenlibs maeparser)

target_compile_definitions(${PROJECT_NAME} PRIVATE COORDGENLIBS_LIBRARY)