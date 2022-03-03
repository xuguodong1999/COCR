project(openbabel LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${DEV_INCLUDE_DIR})

if (NOT ANDROID) # use apk_shell for android-qt project
    find_package(Qt5 COMPONENTS Core REQUIRED)
endif ()

include(${CMAKE_SOURCE_DIR}/cmake/macro.cmake)
GET_ALL_SUB_DIR_NAME(PROJECT_SOURCE_DIR_LIST ${CMAKE_CURRENT_SOURCE_DIR})

# require libxml2, TODO: port to QtXml
list(REMOVE_ITEM PROJECT_SOURCE_DIR_LIST src/formats/xml)
if (APPLE OR ANDROID OR WASM)
    # require <rpc/xdr.h> <zlib.h>
    list(REMOVE_ITEM PROJECT_SOURCE_DIR_LIST src/formats/xtc src/formats/png)
endif ()

foreach (PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR_LIST})
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_SOURCE_DIR} PROJECT_SOURCES)
    list(APPEND PROJECT_SOURCE ${PROJECT_SOURCES})
endforeach ()

# include windows dll info
if (MSVC)
    list(APPEND PROJECT_SOURCE ${DEV_ASSETS_DIR}/windows/3rd.rc)
    include_directories(${DEV_ASSETS_DIR}/windows)
endif ()

# generate openbabel qrc
include(${CMAKE_SOURCE_DIR}/cmake/GenerateOpenBabelQrc.cmake)
generateOpenBabelQrc(${OpenBabel_QRC})

add_library(${PROJECT_NAME} ${PROJECT_SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core coordgenlibs maeparser inchi)
if (IOS OR ANDROID OR APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenMP_CXX_FLAGS})
endif ()