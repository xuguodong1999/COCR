project(cocr LANGUAGES CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${openbabel_INCLUDE_DIR})
include_directories(${coordgenlibs_INCLUDE_DIR})

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(OpenCV REQUIRED)
find_package(ncnn REQUIRED)

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

makeDLL(base)

makeDLL(chem)
addDeps(chem base)
add3rdDeps(chem "openbabel;coordgenlibs")

makeDLL(stroke)
addDeps(stroke "base;chem")
add3rdDeps(stroke "opencv_core;opencv_imgproc")
target_compile_definitions(${PROJECT_NAME}_stroke PRIVATE
        "TEST_SAMPLES_PATH=std::string(\"${DEV_ASSETS_DIR}/\")")

makeDLL(data)
addDeps(data "base;chem;stroke")
add3rdDeps(data "opencv_core;opencv_imgcodecs;opencv_highgui;Qt5::Core;Qt5::Widgets")
target_compile_definitions(${PROJECT_NAME}_data PRIVATE
        "TEST_SAMPLES_PATH=std::string(\"${DEV_ASSETS_DIR}/\")")

#makeDLL(ocr)
#makeDLL(ncnn_crnn)
## makeDLL(ocv_crnn)
#addDeps(ocr base chem)
#
#if (USE_OPENCV_DNN)
#    makeDLL(ocv_yolo)
#else ()
#    makeDLL(ncnn_yolo)
#endif ()