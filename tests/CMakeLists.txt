add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/maeparser)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/coordgenlibs)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/rxcpp)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/rxqt)
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/openbabel)

add_dependencies(MaeparserTest maeparser)
add_dependencies(CoordgenlibsTest coordgenlibs)
# add_dependencies(OpenBabelTest openbabel)
