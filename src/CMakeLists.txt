#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/webview)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/leafxy)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd_usage)

add_dependencies(leafxy openbabel)
add_dependencies(leafxy coordgenlibs)