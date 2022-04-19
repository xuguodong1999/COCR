project(tools LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

#find_package(OpenCV QUIET)
#find_package(Torch QUIET)
find_package(Qt5 COMPONENTS Widgets QUIET)

#CHECK_OPENCV(${PROJECT_NAME})
#CHECK_LIBTORCH(${PROJECT_NAME})
CHECK_QT(${PROJECT_NAME} "Widgets")

addExecutable(c2fmt c2fmt.cpp)

addExecutable(fix_js_import fix_js_import.cpp)
linkQt(fix_js_import "Core")

addExecutable(simplify_torch_import simplify_torch_import.cpp)
linkQt(simplify_torch_import "Core")

addExecutable(gemm_count gemm_count.cpp)
