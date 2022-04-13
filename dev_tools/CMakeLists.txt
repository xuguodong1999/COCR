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

makeLibrary(base els_base)

makeLibrary(math els_math)
addLibraryDeps(els_math els_base)

makeLibrary(ceb els_ceb)
addLibraryDeps(els_ceb els_math)

#makeLibrary(ckit els_ckit)
#addLibraryDeps(els_ckit els_base)

makeLibrary(ocv_wrapper els_ocv_wrapper)
addLibraryDeps(els_ocv_wrapper els_base)
linkOpenCV(els_ocv_wrapper)

makeLibrary(stroke els_stroke)
addLibraryDeps(els_stroke els_ocv_wrapper)
