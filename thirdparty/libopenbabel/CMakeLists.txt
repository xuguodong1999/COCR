project(openbabel LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (NOT ANDROID) # use apk_shell for android-qt project
    find_package(Qt5 COMPONENTS Core REQUIRED)
endif ()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src PROJECT_SOURCE)
GET_ALL_SUB_DIR_NAME(PROJECT_SOURCE_DIR_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src)

# require libxml2, TODO: port to QtXml
list(REMOVE_ITEM PROJECT_SOURCE_DIR_LIST formats/xml)
if (NOT UNIX OR APPLE OR ANDROID OR WASM)
    # require <rpc/xdr.h> <zlib.h>
    list(REMOVE_ITEM PROJECT_SOURCE_DIR_LIST formats/xtc formats/png)
endif ()

foreach (PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR_LIST})
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_SOURCE_DIR} PROJECT_SOURCE)
endforeach ()

# include windows dll info
if (MSVC)
    list(APPEND PROJECT_SOURCE ${DEV_ASSETS_DIR}/windows/3rd.rc)
    include_directories(${DEV_ASSETS_DIR}/windows)
endif ()

# generate openbabel qrc
writeQrcFileBySearchDirectory("${openbabel_QRC}" "${DEV_ASSETS_DIR}/openbabel/**" "${DEV_ASSETS_DIR}")

add_library(${PROJECT_NAME} ${PROJECT_SOURCE})
add_dependencies(${PROJECT_NAME} inchi)
add_dependencies(${PROJECT_NAME} maeparser)
add_dependencies(${PROJECT_NAME} coordgenlibs)
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${coordgenlibs_INCLUDE_DIR}
        ${maeparser_INCLUDE_DIR}
        ${inchi_INCLUDE_DIR}
        ${openbabel_INCLUDE_DIR}
        ${DEV_HEADER_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core coordgenlibs maeparser inchi)
if (ANDROID)
    target_link_libraries(${PROJECT_NAME} PRIVATE -static-openmp)
elseif (IOS OR APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
endif ()

if (BUILD_TESTS)
    add_subdirectory(test)
endif ()