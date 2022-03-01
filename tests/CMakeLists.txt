add_subdirectory(maeparser)
add_subdirectory(coordgenlibs)
add_subdirectory(rxcpp)
add_subdirectory(rxqt)
add_subdirectory(openbabel)

add_dependencies(MaeparserTest maeparser)
add_dependencies(CoordgenlibsTest coordgenlibs)
add_dependencies(OpenBabelTest openbabel)
