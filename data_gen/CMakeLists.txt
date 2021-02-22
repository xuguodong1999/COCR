project(data_gen)

set(CMAKE_CXX_STANDARD 17)

include_directories(../openbabel/include ../coordgenlibs/include)

find_package(OpenCV REQUIRED)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

set(CMAKE_AUTORCC ON)

aux_source_directory(. PROJECT_SOURCES)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ../resources/obdata.qrc)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

# link opencv, patched openbabel and coordgen here
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OpenCV_LIBS} Qt${QT_VERSION_MAJOR}::Core openbabel coordgen)
