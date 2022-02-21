project(elslab LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${COCR_INCLUDE_DIR})

find_package(OpenCV REQUIRED)
find_package(Torch REQUIRED)

include_directories(${TORCH_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})

# collect source files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCE)
list(APPEND PROJECT_SOURCE ${OPENBABEL_DATA_QRC_FILE})

# add exe
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE})

# product suffix
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

target_link_libraries(${PROJECT_NAME} PRIVATE ${TORCH_LIBRARIES} ${OpenCV_LIBS})
