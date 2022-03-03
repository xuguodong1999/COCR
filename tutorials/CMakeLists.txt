project(tut LANGUAGES CXX)

include_directories(${DEV_HEADER_DIR})

find_package(Boost QUIET)
find_package(Torch QUIET)
find_package(OpenCV QUIET)
find_package(Qt5 COMPONENTS Widgets QUIET)

# collect source files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src PROJECT_SOURCE)
# {name}.cpp -> target as -> {name}.cpp.o
# rules:
#   if {name} has qt suffix, then link qt
#   if {name} has ob suffix, then link openbabel
#   if {name} has torch suffix, then link libtorch
foreach (PROJECT_SOURCE_FILE_PATH ${PROJECT_SOURCE})
    # add exe
    get_filename_component(PROJECT_SOURCE_FILE ${PROJECT_SOURCE_FILE_PATH} NAME)
    set(TARGET_NAME ${PROJECT_NAME}_${PROJECT_SOURCE_FILE})
    # file prefix
    ## ob: try build openbabel with boost, coordgenlibs and maeparser
    string(FIND ${PROJECT_SOURCE_FILE} "ob" PREFIX_POS)
    if (NOT PREFIX_POS) # ==0
        if (NOT ${Boost_FOUND})
            message(WARNING "Boost not found, ${TARGET_NAME} will not be fully built")
            continue()
        endif ()
        add_executable(${TARGET_NAME} ${PROJECT_SOURCE_FILE_PATH})
        # product suffix
        set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "d")
        target_include_directories(${TARGET_NAME} PRIVATE ${Boost_INCLUDE_DIR} ${DEV_INCLUDE_DIR})
        target_link_libraries(${TARGET_NAME} openbabel coordgenlibs maeparser ${Boost_LIBRARIES})
        add_dependencies(${TARGET_NAME} openbabel coordgenlibs maeparser)
        continue()
    endif ()
    ## torch
    string(FIND ${PROJECT_SOURCE_FILE} "torch" PREFIX_POS)
    if (NOT PREFIX_POS) # ==0
        if (NOT ${Torch_FOUND})
            message(WARNING "Torch not found, ${TARGET_NAME} will not be built")
            continue()
        endif ()
        if (NOT ${OpenCV_FOUND})
            message(WARNING "OpenCV not found, ${TARGET_NAME} will not be fully built")
            continue()
        endif ()
        add_executable(${TARGET_NAME} ${PROJECT_SOURCE_FILE_PATH})
        # product suffix
        set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "d")
        target_include_directories(${TARGET_NAME} PRIVATE ${TORCH_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} ${DEV_INCLUDE_DIR})
        target_link_libraries(${TARGET_NAME} PRIVATE ${TORCH_LIBRARIES} ${OpenCV_LIBS})
        continue()
    endif ()
    ## qt
    string(FIND ${PROJECT_SOURCE_FILE} "qt" PREFIX_POS)
    if (NOT PREFIX_POS) # ==0
        if (NOT ${Qt5_FOUND})
            message(WARNING "Qt5 not found, ${TARGET_NAME} will not be built")
            continue()
        endif ()
        add_executable(${TARGET_NAME} ${PROJECT_SOURCE_FILE_PATH})
        # product suffix
        set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "d")
        target_link_libraries(${TARGET_NAME} PRIVATE Qt5::Widgets)
        continue()
    endif ()
    add_executable(${TARGET_NAME} ${PROJECT_SOURCE_FILE_PATH})
    # product suffix
    set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "d")
endforeach ()

# standalone proj
add_subdirectory(cpprl)
add_subdirectory(libtorch)