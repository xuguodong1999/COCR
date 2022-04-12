project(ref LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

include_directories(${DEV_HEADER_DIR})

find_package(Boost QUIET)
find_package(Torch QUIET)
find_package(OpenCV QUIET)
find_package(Qt5 COMPONENTS Widgets QUIET)

# collect source files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCE)
# {name}.cpp -> target as -> {name}.cpp.o
# rules:
#   if {name} has qt suffix, then link qt
#   if {name} has ob suffix, then link openbabel
#   if {name} has torch suffix, then link libtorch
foreach (PROJECT_SOURCE_FILE_PATH ${PROJECT_SOURCE})
    # add exe
    get_filename_component(PROJECT_SOURCE_FILE ${PROJECT_SOURCE_FILE_PATH} NAME_WE)
    set(TARGET_NAME ${PROJECT_NAME}_com_${PROJECT_SOURCE_FILE})
    # file prefix
    ## ob: try build openbabel with boost, coordgenlibs and maeparser
    string(FIND "${PROJECT_SOURCE_FILE}" "ob" PREFIX_POS)
    if (NOT PREFIX_POS) # ==0
        CHECK_CONTINUE_BOOST(${TARGET_NAME})
        CHECK_CONTINUE_QT(${TARGET_NAME} "Widgets")
        set(CMAKE_AUTORCC ON)
        addExecutable(${TARGET_NAME} "${PROJECT_SOURCE_FILE_PATH};${openbabel_QRC}")
        linkOpenBabel(${TARGET_NAME})
        linkCoordgenlibs(${TARGET_NAME})
        linkMaeparser(${TARGET_NAME})
        linkQt(${TARGET_NAME} "Widgets")
        linkBoost(${TARGET_NAME})
        continue()
    endif ()
    ## torch
    string(FIND "${PROJECT_SOURCE_FILE}" "torch" PREFIX_POS)
    if (NOT PREFIX_POS) # ==0
        CHECK_CONTINUE_LIBTORCH(${TARGET_NAME})
        CHECK_CONTINUE_OPENCV(${TARGET_NAME})
        addExecutable(${TARGET_NAME} "${PROJECT_SOURCE_FILE_PATH}")
        linkOpenCV(${TARGET_NAME})
        linkLibtorch(${TARGET_NAME})
        continue()
    endif ()
    ## qt
    string(FIND "${PROJECT_SOURCE_FILE}" "qt" PREFIX_POS)
    if (NOT PREFIX_POS) # ==0
        CHECK_CONTINUE_QT(${TARGET_NAME} "Widgets")
        addExecutable(${TARGET_NAME} "${PROJECT_SOURCE_FILE_PATH}")
        linkQt(${TARGET_NAME} "Widgets")
        continue()
    endif ()
    addExecutable(${TARGET_NAME} "${PROJECT_SOURCE_FILE_PATH}")
endforeach ()