if (NOT OpenCV_FOUND)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenCV.cmake)
endif ()
find_package(Torch REQUIRED)