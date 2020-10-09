project(train)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR} SRC_FILES)

set(EXECUTABLE_NAME ${PROJECT_NAME})

add_executable(${EXECUTABLE_NAME} ${SRC_FILES})

if (MSVC)
    target_compile_options(${EXECUTABLE_NAME} PRIVATE -MT)
    if (CMAKE_BUILD_TYPE)
        if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
            target_compile_options(${EXECUTABLE_NAME} PRIVATE -MTd)
        endif ()
    endif (CMAKE_BUILD_TYPE)
endif (MSVC)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        ${OpenBabel_LIBS}
        ${OpenCV_LIBS}
        ${Boost_LIBRARIES}
        ${TORCH_LIBRARIES})

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES)

if (MSVC)
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE
            ${CMAKE_CURRENT_BINARY_DIR})
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG
            ${CMAKE_CURRENT_BINARY_DIR})
endif (MSVC)
