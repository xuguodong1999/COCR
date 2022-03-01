project(elslab LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${DEV_INCLUDE_DIR})

find_package(OpenCV REQUIRED)
find_package(Torch REQUIRED)

include_directories(${TORCH_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})

# collect source files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCE)
foreach (PROJECT_SOURCE_FILE_PATH ${PROJECT_SOURCE})
    # add exe
    get_filename_component(PROJECT_SOURCE_FILE ${PROJECT_SOURCE_FILE_PATH} NAME)
    set(PROJECT_NAME_${PROJECT_NAME} ${PROJECT_NAME}_${PROJECT_SOURCE_FILE})
    add_executable(${PROJECT_NAME_${PROJECT_NAME}} ${PROJECT_SOURCE_FILE_PATH})
    # product suffix
    set_target_properties(${PROJECT_NAME_${PROJECT_NAME}} PROPERTIES DEBUG_POSTFIX "d")
    target_link_libraries(${PROJECT_NAME_${PROJECT_NAME}} PRIVATE ${TORCH_LIBRARIES} ${OpenCV_LIBS})
    unset(PROJECT_NAME_${PROJECT_NAME})
endforeach ()
