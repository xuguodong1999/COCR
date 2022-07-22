include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

find_package(OpenCV REQUIRED)
find_package(Torch QUIET)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

## els modules are linked with PUBLIC config
## els and 3rdparty modules are linked with PRIVATE config

# base
makeLibrary(libbase els_base)

# math
makeLibrary(libmath els_math)
addLibraryDeps(els_math els_base)

# balance
makeLibrary(libceb els_ceb)
addLibraryDeps(els_ceb els_math)

# chem kit
makeLibrary(libckit els_ckit)
addLibraryDeps(els_ckit els_math)
linkOpenBabel(els_ckit)
linkCoordgenlibs(els_ckit)

# opencv wrapper
makeLibrary(libocv els_ocv)
addLibraryDeps(els_ocv els_base)
linkOpenCV(els_ocv)
linkQt(els_ocv "Gui") # access to QImage

# stroke
makeLibrary(libstroke els_stroke)
addLibraryDeps(els_stroke els_ocv)

# ocr module
makeLibrary(libcocr els_cocr)
addLibraryDeps(els_cocr els_ckit)
addLibraryDeps(els_cocr els_ocv)
linkOpenCV(els_cocr)
linkQt(els_cocr "Gui") # access to QImage
if (USE_OPENCV_DNN)
    target_compile_definitions(els_cocr PRIVATE USE_OPENCV_DNN)
else ()
    linkNcnn(els_cocr)
endif ()
if (BUILD_PRIVATE)
    target_compile_definitions(els_cocr PRIVATE
            MODEL_DIR=\"${DEV_ASSETS_DIR}/models\")
else ()
    target_compile_definitions(els_cocr PRIVATE
            MODEL_DIR=\":/models\")
endif ()

# data module
makeLibrary(libdata els_data)
addLibraryDeps(els_data els_stroke)
addLibraryDeps(els_data els_ocv)
addLibraryDeps(els_data els_ckit)
addLibraryDeps(els_data els_math)
linkOpenBabel(els_data)
linkCoordgenlibs(els_data)
linkQt(els_data "Gui")

# nn module
if (Torch_FOUND)
    makeLibrary(libnn els_nn)
    addLibraryDeps(els_nn els_base)
    addLibraryDeps(els_nn els_math)
    addLibraryDeps(els_nn els_ocv)
    linkLibtorch(els_nn)
    linkOpenCV(els_nn)
    linkQt(els_nn "Gui")
endif (Torch_FOUND)

# executable
add_subdirectory(app)
if (BUILD_PRIVATE)
    add_subdirectory(app_qml)
endif ()
if (BUILD_DATA_GEN)
    add_subdirectory(data_gen)
endif ()
add_subdirectory(examples)
add_subdirectory(tools)
