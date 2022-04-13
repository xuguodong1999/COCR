project(dev_tools LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

add_subdirectory(app_qml)
add_subdirectory(standalone)

makeLibrary(base els_base)

makeLibrary(math els_math)
addLibraryDeps(els_math els_base)

#makeLibrary(ckit els_ckit)
#addLibraryDeps(els_ckit els_base)
