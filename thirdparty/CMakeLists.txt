add_subdirectory(libncnn)
add_subdirectory(libinchi)
add_subdirectory(libcoordgenlibs)
add_subdirectory(libmaeparser)
add_subdirectory(libopenbabel)

add_subdirectory(librxcpp)
add_subdirectory(librxqt)

if (WITH_LIBTORCH AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/libpnnx)
    add_subdirectory(libpnnx)
endif ()

if (BUILD_REFERENCES)
    add_subdirectory(ref-common)
    add_subdirectory(ref-cpprl)
    add_subdirectory(ref-libtorch)
endif ()