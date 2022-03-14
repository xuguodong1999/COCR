add_subdirectory(coordgenlibs)
add_subdirectory(maeparser)
add_subdirectory(openbabel)
add_subdirectory(inchi)

add_dependencies(coordgenlibs maeparser)

add_dependencies(openbabel inchi)
add_dependencies(openbabel maeparser)
add_dependencies(openbabel coordgenlibs)