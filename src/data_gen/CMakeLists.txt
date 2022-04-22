project(data_gen)
include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(OpenCV REQUIRED)

addExecutable(${PROJECT_NAME} "data_gen.cpp")

addLibraryDeps(${PROJECT_NAME} els_base)
addLibraryDeps(${PROJECT_NAME} els_data)
addLibraryDeps(${PROJECT_NAME} els_stroke)

linkQt(${PROJECT_NAME} "Widgets")
linkOpenCV(${PROJECT_NAME})