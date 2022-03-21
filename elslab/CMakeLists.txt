project(elslab LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(OpenCV QUIET)
find_package(Torch QUIET)

if (NOT ${Torch_FOUND})
    message(WARNING "Torch not found, ${PROJECT_NAME} will not be built")
    return()
endif ()

if (NOT ${OpenCV_FOUND})
    message(WARNING "OpenCV not found, ${PROJECT_NAME} will not be built")
    return()
endif ()

include_directories(${TORCH_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} ${DEV_HEADER_DIR})

# collect source files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCES)
foreach (PROJECT_SOURCE ${PROJECT_SOURCES})
    # add exe
    get_filename_component(PROJECT_SOURCE_FILE ${PROJECT_SOURCE} NAME_WE)
    set(TARGET_NAME ${PROJECT_NAME}_${PROJECT_SOURCE_FILE})
    add_executable(${TARGET_NAME} ${PROJECT_SOURCE})
    # product suffix
    set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "d")
    target_link_libraries(${TARGET_NAME} PRIVATE ${TORCH_LIBRARIES} ${OpenCV_LIBS})
endforeach ()