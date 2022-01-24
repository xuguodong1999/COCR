#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/webview)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/leafxy)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd_usage)

add_dependencies(leafxy openbabel)
add_dependencies(leafxy coordgenlibs)

if (NOT ANDROID)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/data_gen)
    add_dependencies(data_gen coordgenlibs)
    add_dependencies(data_gen openbabel)
endif ()