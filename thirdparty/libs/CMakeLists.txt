add_subdirectory(ncnn)
add_subdirectory(inchi)
add_subdirectory(coordgenlibs)
add_subdirectory(maeparser)
add_subdirectory(openbabel)

add_subdirectory(rxcpp)
add_subdirectory(rxqt)

if (WITH_LIBTORCH AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/pnnx)
    add_subdirectory(pnnx)
endif ()