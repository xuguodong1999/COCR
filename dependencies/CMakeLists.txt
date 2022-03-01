add_subdirectory(src/coordgenlibs)
add_subdirectory(src/maeparser)
add_subdirectory(src/openbabel)
add_subdirectory(src/inchi)

add_dependencies(coordgenlibs maeparser)

add_dependencies(openbabel inchi)
add_dependencies(openbabel maeparser)
add_dependencies(openbabel coordgenlibs)