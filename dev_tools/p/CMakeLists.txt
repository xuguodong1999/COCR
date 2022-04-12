project(cocr LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

if (NOT ANDROID) # use apk_shell for android-qt project
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
endif ()
find_package(OpenCV REQUIRED)

function(makeDLL TARGET)
    unset(PROJECT_SOURCE)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/${TARGET} PROJECT_SOURCE)
    set(TARGET_NAME ${PROJECT_NAME}_${TARGET})
    addLibrary(${TARGET_NAME} "${PROJECT_SOURCE}")
    # alias xxx to cocr::xxx
    add_library(${PROJECT_NAME}::${TARGET} ALIAS ${TARGET_NAME})
endfunction()

function(addDeps TARGET DEPS)
    foreach (DEP ${DEPS})
        add_dependencies(${PROJECT_NAME}_${TARGET} ${PROJECT_NAME}_${DEP})
        target_link_libraries(${PROJECT_NAME}_${TARGET} PRIVATE ${PROJECT_NAME}_${DEP})
    endforeach ()
endfunction()

# cocr::base
makeDLL(base)

# cocr::chem
makeDLL(chem)
addDeps(chem "base")
linkOpenBabel(${PROJECT_NAME}_chem)
linkCoordgenlibs(${PROJECT_NAME}_chem)

# cocr::opencv_util
makeDLL(opencv_util)
addDeps(opencv_util "base")
linkOpenCV(${PROJECT_NAME}_opencv_util)

if (BUILD_DATA_GEN)
    # cocr::stroke
    makeDLL(stroke)
    addDeps(stroke "base;chem;opencv_util")
    linkOpenCV(${PROJECT_NAME}_stroke)
    target_compile_definitions(${PROJECT_NAME}_stroke PRIVATE
            "TEST_SAMPLES_PATH=\"${DEV_ASSETS_DIR}/\"")

    # cocr::data
    makeDLL(data)
    addDeps(data "base;chem;stroke;opencv_util")
    linkOpenCV(${PROJECT_NAME}_data)
    linkQt(${PROJECT_NAME}_data "Widgets;Core")
    target_compile_definitions(${PROJECT_NAME}_data PRIVATE
            "TEST_SAMPLES_PATH=\"${DEV_ASSETS_DIR}/\"")
endif ()

# cocr::ocr, include header def for ncnn or opencv_dnn's impl plugin
makeDLL(ocr)
addDeps(ocr "base;chem;opencv_util")
linkOpenCV(${PROJECT_NAME}_ocr)
linkQt(${PROJECT_NAME}_ocr "Core")

if (USE_OPENCV_DNN)
    target_compile_definitions(${PROJECT_NAME}_ocr PRIVATE USE_OPENCV_DNN)
    if (USE_OPENVINO)
        target_compile_definitions(${PROJECT_NAME}_ocr PRIVATE USE_OPENVINO)
    endif (USE_OPENVINO)
else ()
    linkNcnn(${PROJECT_NAME}_ocr)
endif ()

if (BUILD_PRIVATE)
    target_compile_definitions(${PROJECT_NAME}_ocr PRIVATE
            MODEL_DIR=\"${DEV_ASSETS_DIR}/models\")
else ()
    target_compile_definitions(${PROJECT_NAME}_ocr PRIVATE
            MODEL_DIR=\":/models\")
endif ()
