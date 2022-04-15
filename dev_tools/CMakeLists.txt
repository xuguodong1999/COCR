project(dev_tools LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

add_subdirectory(app_qml)
add_subdirectory(standalone)

find_package(OpenCV QUIET)
find_package(Torch QUIET)
find_package(Qt5 COMPONENTS Widgets QUIET)

CHECK_OPENCV(${PROJECT_NAME})
CHECK_LIBTORCH(${PROJECT_NAME})
CHECK_QT(${PROJECT_NAME} "Widgets")

## els modules are linked with PUBLIC config
## els and 3rdparty modules are linked with PRIVATE config
# base
makeLibrary(base els_base)
# math
makeLibrary(math els_math)
addLibraryDeps(els_math els_base)
# balance
makeLibrary(ceb els_ceb)
addLibraryDeps(els_ceb els_math)
# chem kit
makeLibrary(ckit els_ckit)
addLibraryDeps(els_ckit els_math)
linkOpenBabel(els_ckit)
linkCoordgenlibs(els_ckit)
# opencv wrapper
makeLibrary(ocv els_ocv)
addLibraryDeps(els_ocv els_base)
linkOpenCV(els_ocv)
linkQt(els_ocv "Gui") # access to QImage
# stroke
makeLibrary(stroke els_stroke)
addLibraryDeps(els_stroke els_ocv)
# ocr module
makeLibrary(cocr els_cocr)
addLibraryDeps(els_cocr els_ckit)
addLibraryDeps(els_cocr els_ocv)
linkOpenCV(els_cocr)
linkQt(els_cocr "Core")
if (NOT USE_OPENCV_DNN)
    linkNcnn(els_cocr)
endif ()
if (BUILD_PRIVATE)
    target_compile_definitions(els_cocr PRIVATE
            MODEL_DIR=\"${DEV_ASSETS_DIR}/models\")
else ()
    target_compile_definitions(els_cocr PRIVATE
            MODEL_DIR=\":/models\")
endif ()
