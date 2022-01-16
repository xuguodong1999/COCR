add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/webview)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/coordgenlibs)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/maeparser)

add_dependencies(coordgenlibs maeparser)