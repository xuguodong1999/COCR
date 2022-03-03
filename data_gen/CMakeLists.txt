project(data_gen LANGUAGES CXX)

set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${DEV_INCLUDE_DIR})

set(QT_COMPONENTS Widgets Core)
find_package(Qt5 COMPONENTS ${QT_COMPONENTS} REQUIRED)
find_package(OpenCV REQUIRED)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)

# collect source files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCE)
list(APPEND PROJECT_SOURCE ${OpenBabel_QRC})

# add exe
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE})

# product suffix
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

set(QT_COMPONENT_TARGETS)
foreach (QT_COMPONENT ${QT_COMPONENTS})
    list(APPEND QT_COMPONENT_TARGETS Qt5::${QT_COMPONENT})
endforeach ()

target_link_libraries(${PROJECT_NAME} PRIVATE
        openbabel coordgenlibs
        ${QT_COMPONENT_TARGETS}
        opencv_core opencv_imgproc opencv_imgcodecs opencv_highgui)

if (IOS OR ANDROID OR APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenMP_CXX_FLAGS})
endif ()

target_compile_definitions(${PROJECT_NAME} PRIVATE "TEST_SAMPLES_PATH=std::string(\"${DEV_ASSETS_DIR}/datasets/\")")

if (MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE "/FORCE:MULTIPLE")
endif (MSVC)