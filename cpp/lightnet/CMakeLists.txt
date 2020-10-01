set(MODULE_NAME lightnet)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/" ${CMAKE_MODULE_PATH})

include(CheckLanguage)
check_language(CUDA)
enable_language(CUDA)

find_package(CUDA REQUIRED)
cuda_select_nvcc_arch_flags(CUDA_ARCH_FLAGS "Common")
message(STATUS "Building with CUDA flags: " "${CUDA_ARCH_FLAGS}")
set(CMAKE_CUDA_RUNTIME_LIBRARY "Shared")
find_package(CUDNN)

if (WIN32)
    set(PThreads_windows_DIR ${CMAKE_CURRENT_LIST_DIR}/backened/darknet/3rdparty/pthreads)
endif ()


set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_THREAD_PREFER_PTHREAD ON)

find_package(Threads REQUIRED)
if (MSVC)
    find_package(PThreads_windows REQUIRED)
endif ()

find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()

include_directories(${CMAKE_CURRENT_LIST_DIR}/backened/darknet/include)

file(GLOB darknet_src "${CMAKE_CURRENT_LIST_DIR}/backened/darknet/src/*.cpp")
file(GLOB darknet_cuda_src "${CMAKE_CURRENT_LIST_DIR}/backened/darknet/src/*.cu")

if (NOT MSVC)
    list(REMOVE_ITEM darknet_src
            ${CMAKE_CURRENT_LIST_DIR}/backened/darknet/src/gettimeofday.cpp
            ${CMAKE_CURRENT_LIST_DIR}/backened/darknet/src/getopt.cpp)
endif ()
add_executable(${MODULE_NAME} ${darknet_src} ${darknet_cuda_src})

if (MSVC)
    target_link_libraries(${MODULE_NAME} PRIVATE ws2_32 PThreads_windows::PThreads_windows)
    target_compile_definitions(${MODULE_NAME} PRIVATE -D_CRT_RAND_S -DNOMINMAX -D_USE_MATH_DEFINES -D_TIMESPEC_DEFINED)
endif ()

target_link_libraries(${MODULE_NAME} PRIVATE ${OpenCV_LIBS} Threads::Threads curand cublas cuda CuDNN::CuDNN)
target_compile_definitions(${MODULE_NAME} PRIVATE -DGPU -DCUDNN -DCUDNN_HALF -DOPENCV -DUSE_CMAKE_LIBS)
