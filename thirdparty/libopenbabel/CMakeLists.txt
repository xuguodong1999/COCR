project(openbabel LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

if (NOT ANDROID) # use apk_shell for android-qt project
    find_package(Qt5 COMPONENTS Core REQUIRED)
endif ()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

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

# generate openbabel qrc
writeQrcFileBySearchDirectory(
        "${openbabel_QRC}"
        "${CMAKE_CURRENT_SOURCE_DIR}/data/**"
        "${CMAKE_CURRENT_SOURCE_DIR}/data")

addLibrary(${PROJECT_NAME} "${PROJECT_SOURCE}")
linkInchi(${PROJECT_NAME})
linkMaeparser(${PROJECT_NAME})
linkCoordgenlibs(${PROJECT_NAME})
linkQt(${PROJECT_NAME} Core)
linkOpenMP(${PROJECT_NAME})

if (BUILD_TESTS)
    add_subdirectory(test)
endif ()