project(data_gen LANGUAGES CXX)

set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${COCR_INCLUDE_DIR})

set(QT_COMPONENTS Widgets Core)
find_package(QT NAMES Qt6 Qt5 COMPONENTS ${QT_COMPONENTS} REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_COMPONENTS} REQUIRED)
find_package(OpenCV REQUIRED)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)

# collect source files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCE)
list(APPEND PROJECT_SOURCE ${OPENBABEL_DATA_QRC_FILE})

# add exe
if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCE})
else ()
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCE})
endif ()

# product suffix
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

set(QT_COMPONENT_TARGETS)
foreach (QT_COMPONENT ${QT_COMPONENTS})
    list(APPEND QT_COMPONENT_TARGETS Qt${QT_VERSION_MAJOR}::${QT_COMPONENT})
endforeach ()

target_link_libraries(${PROJECT_NAME} PRIVATE
        openbabel coordgenlibs
        ${QT_COMPONENT_TARGETS}
        opencv_core opencv_imgproc opencv_imgcodecs opencv_highgui)

target_compile_definitions(${PROJECT_NAME} PRIVATE "TEST_SAMPLES_PATH=std::string(\"${COCR_DATA_DIR}/datasets/\")")

# openmp for apple clang
if (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
endif(APPLE)

if (MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE "/FORCE:MULTIPLE")
endif (MSVC)