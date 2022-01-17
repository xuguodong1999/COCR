#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/webview)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/coordgenlibs)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/maeparser)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/openbabel)

add_dependencies(coordgenlibs maeparser)
add_dependencies(openbabel maeparser)
add_dependencies(openbabel coordgenlibs)