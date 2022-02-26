project(openbabel LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${COCR_INCLUDE_DIR})
include_directories(${COCR_HEADER_DIR}/../include/inchi)

set(QT_COMPONENTS Core)
if (NOT ANDROID) # use apk_shell for android-qt project
    find_package(Qt5 COMPONENTS ${QT_COMPONENTS} REQUIRED)
endif ()

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
if (UNIX AND NOT APPLE AND NOT ANDROID)
    list(APPEND PROJECT_SOURCE_DIR_LIST src/formats/xtc src/formats/png)
endif ()
set(PROJECT_SOURCE)
foreach (PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR_LIST})
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_SOURCE_DIR} PROJECT_SOURCES)
    list(APPEND PROJECT_SOURCE ${PROJECT_SOURCES})
endforeach ()

# include windows dll info
if (MSVC)
    list(APPEND PROJECT_SOURCE ${COCR_ASSETS_DIR}/windows/3rd.rc)
    include_directories(${COCR_ASSETS_DIR}/windows)
endif ()

# generate openbabel qrc
set(OPENBABEL_DATA_DIR ${COCR_ASSETS_DIR}/openbabel)
file(GLOB_RECURSE OPENBABEL_DATA_LIST ${OPENBABEL_DATA_DIR}/**)
set(OPENBABEL_QRC "")
string(APPEND OPENBABEL_QRC "<RCC>\n<qresource prefix=\"/\">\n")
foreach (OPENBABEL_DATA ${OPENBABEL_DATA_LIST})
    string(REPLACE ${COCR_ASSETS_DIR}/ "" OPENBABEL_DATA_RELATIVE_PATH ${OPENBABEL_DATA})
    string(APPEND OPENBABEL_QRC
            "<file compress=\"9\" compress-algo=\"best\">${OPENBABEL_DATA_RELATIVE_PATH}</file>\n")
endforeach ()
string(APPEND OPENBABEL_QRC "</qresource>\n</RCC>")
file(WRITE ${OpenBabel_QRC} ${OPENBABEL_QRC})

add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

set(QT_COMPONENT_TARGETS)
foreach (QT_COMPONENT ${QT_COMPONENTS})
    list(APPEND QT_COMPONENT_TARGETS Qt5::${QT_COMPONENT})
endforeach ()

target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_COMPONENT_TARGETS} coordgenlibs maeparser)

# openmp for apple clang
if (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
endif (APPLE)

target_compile_definitions(${PROJECT_NAME} PRIVATE OPENBABEL_LIBRARY)