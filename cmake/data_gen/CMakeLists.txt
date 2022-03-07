project(data_gen LANGUAGES CXX)

set(CMAKE_AUTORCC ON)

include_directories(${DEV_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/tools)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenCV REQUIRED)

# collect source files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src PROJECT_SOURCE)

# add exe
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE} ${OpenBabel_QRC})

# product suffix
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

target_link_libraries(${PROJECT_NAME} PRIVATE
        openbabel coordgenlibs
        Qt5::Widgets
        opencv_core opencv_imgproc opencv_imgcodecs opencv_highgui)

if (IOS OR ANDROID OR APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenMP_CXX_FLAGS})
endif ()

target_compile_definitions(${PROJECT_NAME} PRIVATE "TEST_SAMPLES_PATH=std::string(\"${DEV_ASSETS_DIR}/datasets/\")")

if (MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE "/FORCE:MULTIPLE")
endif (MSVC)