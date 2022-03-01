project(tut LANGUAGES CXX)

include_directories(${DEV_HEADER_DIR})

# collect source files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCE)
# {name}.cpp -> target as -> {name}.cpp.o
# rules:
#   if {name} has qt suffix, then link qt
#   if {name} has ob suffix, then link openbabel
foreach (PROJECT_SOURCE_FILE_PATH ${PROJECT_SOURCE})
    # add exe
    get_filename_component(PROJECT_SOURCE_FILE ${PROJECT_SOURCE_FILE_PATH} NAME)
    set(TARGET_NAME ${PROJECT_NAME}_${PROJECT_SOURCE_FILE})
    add_executable(${TARGET_NAME} ${PROJECT_SOURCE_FILE_PATH})
    # product suffix
    set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "d")
#    target_link_libraries(${TARGET_NAME} PRIVATE ${TORCH_LIBRARIES} ${OpenCV_LIBS})
endforeach ()

# standalone proj
add_subdirectory(cpprl)
add_subdirectory(libtorch)