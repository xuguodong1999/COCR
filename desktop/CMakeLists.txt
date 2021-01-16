cmake_minimum_required(VERSION 3.16)

project(cocr-desktop)

# this flag determines building strategy for opencv and libtorch.
# if set to false, libtorch won't be added and opencv will be cpu-only,
# which means training program won't be build.
# app will use opencv as dnn engine, instead of libtorch
set(ALLOW_USER_TRAIN ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_DEBUG_POSTFIX d)

if (MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    set(INSTALL_DIR ${CMAKE_SOURCE_DIR}/../win/install)
endif (MSVC)

# openmp for alkane generation acceleration
find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else (OPENMP_FOUND)
    message(WARNING "openmp not found. Acceleration for alkane generation disabled.")
endif (OPENMP_FOUND)

# qt5 for UI and cpp framework
# set your own qt install path here, default should be
# C:/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5 on windows
# below is my qt path
if (MSVC)
    set(Qt5_DIR C:/shared/qt5.15.2/lib/cmake/Qt5)
elseif (APPLE)
    set(Qt5_DIR xxx)
elseif (UNIX)
    # ubuntu
    set(Qt5_DIR /home/xgd/shared/Qt/5.15.2/gcc_64/lib/cmake/Qt5)
    # wsl
    # set(Qt5_DIR /home/xgd/Qt/5.15.1/gcc_64/lib/cmake/Qt5)
endif ()
# if qt5 not found, meet fatal error here
include(${CMAKE_SOURCE_DIR}/cmake/SetupQt5.cmake)

# torch for crnn, yolo-related experiments and final papers
if (ALLOW_USER_TRAIN)
    # cuda for torch
    set(CMAKE_CUDA_STANDARD 14)
    set(CMAKE_CUDA_STANDARD_REQUIRED TRUE)
    # path for my build
    if (MSVC)
        set(SHARED_LIBS_DIR C:/shared)
        set(OpenCV_DIR ${SHARED_LIBS_DIR}/opencv4.5.0/x64/vc16/lib)
        set(Torch_DIR ${SHARED_LIBS_DIR}/libtorch1.7.0/share/cmake/Torch)
    elseif (APPLE)
        message(FATAL "dont support libtorch train on macOS")
    elseif (UNIX)
        set(SHARED_LIBS_DIR /home/xgd/shared)
        # ubuntu use self-build opencv
        set(OpenCV_DIR /home/xgd/shared/opencv4.5.1/lib/cmake/opencv4)
        set(Torch_DIR ${SHARED_LIBS_DIR}/libtorch1.7.1/share/cmake/Torch)
        # wsl use opencv from apt-get
        # set(Torch_DIR ${SHARED_LIBS_DIR}/libtorch/share/cmake/Torch)
    endif (MSVC)
    find_package(OpenCV REQUIRED)
    find_package(Torch REQUIRED)

    include_directories(${TORCH_INCLUDE_DIRS})
    include_directories(${OpenCV_INCLUDE_DIRS})

endif (ALLOW_USER_TRAIN)

if (MSVC)
    # openbabel for 3d molecule pos generation
    include(${CMAKE_SOURCE_DIR}/cmake/OpenBabelConfig.cmake)
    # coordgen for 2d molecule pos generation
    include(${CMAKE_SOURCE_DIR}/cmake/CoordgenConfig.cmake)
    # opencv for using yolo in app
    if (NOT ALLOW_USER_TRAIN)
        include(${CMAKE_SOURCE_DIR}/cmake/OpenCVConfig.cmake)
    endif ()
elseif (APPLE)
    message(FATAL "TODO: ADD CMAKE CONFIG ON MACOS")
elseif (UNIX)
    # install qt5 from maintenance tool and use apt-get to install other libs
    # sudo apt-get install libschroedinger-coordgenlibs-dev
    # sudo apt-get install libopenbabel-dev librdkit-dev 
    # wsl: sudo apt-get install libopencv-dev
    include(${CMAKE_SOURCE_DIR}/cmake/SetupUbuntu.cmake)
endif (MSVC)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
include_directories(${SRC_DIR})
include_directories(${INSTALL_DIR}/include)

# client side app with opencv dnn engine backend
add_subdirectory(app)
add_subdirectory(test_ui)
add_subdirectory(test_3d)
if (ALLOW_USER_TRAIN)
    add_subdirectory(train_cifar)
    if (UNIX AND NOT APPLE)
        add_subdirectory(qdarknet)
    endif ()
endif (ALLOW_USER_TRAIN)
