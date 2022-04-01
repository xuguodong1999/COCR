project(cocr LANGUAGES CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${openbabel_INCLUDE_DIR})
include_directories(${coordgenlibs_INCLUDE_DIR})

if (NOT ANDROID) # use apk_shell for android-qt project
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
endif ()
find_package(OpenCV REQUIRED)

#find_package(ncnn REQUIRED)
## ncnn has not put its include directory in cmake config yet
#include_directories(${ncnn_DIR}/../../../include)
#if (ANDROID)
#    # override no rtti and exceptions in ncnn
#    set_target_properties(ncnn PROPERTIES INTERFACE_COMPILE_OPTIONS "-fexceptions")
#endif ()

include(GenerateExportHeader)
function(makeDLL TARGET)
    unset(PROJECT_SOURCE)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/${TARGET} PROJECT_SOURCE)
    # include windows dll info
    if (MSVC)
        list(APPEND PROJECT_SOURCE ${DEV_ASSETS_DIR}/windows/3rd.rc)
        include_directories(${DEV_ASSETS_DIR}/windows)
    endif ()
    set(TARGET_NAME ${PROJECT_NAME}_${TARGET})
    add_library(${TARGET_NAME} ${PROJECT_SOURCE})
    add_library(${PROJECT_NAME}::${TARGET} ALIAS ${TARGET_NAME})
    set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "d")
    generate_export_header(${TARGET_NAME})
    target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

function(addDeps TARGET DEPS)
    foreach (DEP ${DEPS})
        add_dependencies(${PROJECT_NAME}_${TARGET} ${PROJECT_NAME}_${DEP})
        target_link_libraries(${PROJECT_NAME}_${TARGET} PRIVATE ${PROJECT_NAME}_${DEP})
    endforeach ()
endfunction()

function(add3rdDeps TARGET DEPS)
    foreach (DEP ${DEPS})
        target_link_libraries(${PROJECT_NAME}_${TARGET} PRIVATE ${DEP})
    endforeach ()
endfunction()

# cocr::base
makeDLL(base)

# cocr::chem
makeDLL(chem)
addDeps(chem "base")
add3rdDeps(chem "openbabel;coordgenlibs")

# cocr::opencv_util
makeDLL(opencv_util)
addDeps(opencv_util "base")
add3rdDeps(opencv_util "opencv_core;opencv_imgproc")

if (BUILD_DATA_GEN)
    # cocr::stroke
    makeDLL(stroke)
    addDeps(stroke "base;chem;opencv_util")
    add3rdDeps(stroke "opencv_core;opencv_imgproc")
    target_compile_definitions(${PROJECT_NAME}_stroke PRIVATE
            "TEST_SAMPLES_PATH=std::string(\"${DEV_ASSETS_DIR}/\")")

    # cocr::data
    makeDLL(data)
    addDeps(data "base;chem;stroke;opencv_util")
    add3rdDeps(data "opencv_core;opencv_imgcodecs;opencv_highgui;Qt5::Core;Qt5::Widgets")
    target_compile_definitions(${PROJECT_NAME}_data PRIVATE
            "TEST_SAMPLES_PATH=std::string(\"${DEV_ASSETS_DIR}/\")")
endif ()

# cocr::ocr, include header def for ncnn or opencv_dnn's impl plugin
makeDLL(ocr)
addDeps(ocr "base;chem;opencv_util")
add3rdDeps(ocr "opencv_core;opencv_imgproc;Qt5::Core")

if (USE_OPENCV_DNN)
    add3rdDeps(ocr "opencv_dnn")
    target_compile_definitions(${PROJECT_NAME}_ocr PRIVATE USE_OPENCV_DNN)
    if (USE_OPENVINO)
        target_compile_definitions(${PROJECT_NAME}_ocr PRIVATE USE_OPENVINO)
    endif (USE_OPENVINO)
else ()
    add3rdDeps(ocr "ncnn")
    add_dependencies(cocr_ocr ncnn)
    target_include_directories(cocr_ocr PRIVATE ${ncnn_INCLUDE_DIR})
endif ()
