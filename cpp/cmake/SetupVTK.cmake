if (NOT QT5_FOUND)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupQt5.cmake)
endif ()
find_package(VTK REQUIRED)
