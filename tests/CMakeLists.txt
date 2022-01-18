add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/maeparser)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/coordgenlibs)

add_dependencies(MaeparserTest maeparser)
add_dependencies(CoordgenlibsTest coordgenlibs)