project(ncnn_pnnx)

if (POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif ()

option(PNNX_COVERAGE "build for coverage" OFF)

find_package(Torch REQUIRED)
find_package(TorchVision QUIET)

# torchvision install with brew need linking python, disable it
if (TorchVision_FOUND AND NOT APPLE)
    message(STATUS "Building with TorchVision")
    add_definitions(-DPNNX_TORCHVISION)
else ()
    message(STATUS "Building without TorchVision")
endif ()

add_subdirectory(src)
