#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/webview)
add_subdirectory(leafxy)
add_subdirectory(tutorials)

add_dependencies(leafxy openbabel)
add_dependencies(leafxy coordgenlibs)

if (NOT ANDROID)
    add_subdirectory(data_gen)
    add_dependencies(data_gen coordgenlibs)
    add_dependencies(data_gen openbabel)
endif ()

if (BUILD_ELS_LAB)
    add_subdirectory(elslab)
endif ()