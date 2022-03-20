project(cocr LANGUAGES CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${openbabel_INCLUDE_DIR})
include_directories(${coordgenlibs_INCLUDE_DIR})

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(OpenCV REQUIRED)
find_package(ncnn REQUIRED)

include_directories(${ncnn_DIR}/../../../include)

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

# cocr::ocr, include header def for ncnn or opencv_dnn's impl plugin
makeDLL(ocr)
addDeps(ocr "base;chem;opencv_util")
add3rdDeps(ocr "opencv_core")

# ncnn crnn impl, call by plugin, not header import
makeDLL(ncnn_crnn)
addDeps(ncnn_crnn "ocr")
add3rdDeps(ncnn_crnn "ncnn;Qt5::Core;opencv_imgproc")

# ncnn yolo impl, call by plugin, not header import
makeDLL(ncnn_yolo)
addDeps(ncnn_yolo "ocr")
add3rdDeps(ncnn_yolo "ncnn;Qt5::Core;opencv_imgproc")

# opencv_dnn crnn impl, call by plugin, not header import
makeDLL(ocv_crnn)
addDeps(ocv_crnn "ocr")
add3rdDeps(ocv_crnn "opencv_dnn;Qt5::Core")

# opencv_dnn yolo impl, call by plugin, not header import
makeDLL(ocv_yolo)
addDeps(ocv_yolo "ocr")
add3rdDeps(ocv_yolo "opencv_dnn;Qt5::Core")